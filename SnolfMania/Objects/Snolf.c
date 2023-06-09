#include "GameAPI/Game.h"
#include "Player.h"
#include "HUD.h"
#include "Snolf.h"
#include "Zone.h"

bool32 ShouldRunSnolfCode(EntityPlayer *entity)
{
    // Don't run Snolf code if the player is not in control of the character.
    if (entity->stateInput != Player_Input_P1 && entity->stateInput != Player_Input_P2_Player)
        return false;

    // Don't run Snolf code if the player's state has been forced into the Static state.
    if (entity->state == Player_State_Static)
        return false;

    // Don't run Snolf code if the player is a sidekick/AI-controlled character. (I might want to change this in the future, but there are bugs with end-of-level cutscenes.)
    if (entity->sidekick)
        return false;

    return true;
}

void Snolf_Main(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Bail if controls are locked (e.g, cutscene)
    if (!ShouldRunSnolfCode(entity))
    {
        return;
    }

    // See if forced Snolfing should be allowed (e.g, if an Autoscroller has started.)
    RSDK_GET_ENTITY(SLOT_ZONE, Zone);
    snolfEngine->forceAllow = false;
    if (Zone->autoScrollSpeed)
    {
        snolfEngine->forceAllow = true;
    }

    // Force player to be a ball.
    if (entity->state == Player_State_Ground)
    {
        Player_Action_Roll();
    }

    // Allow the player to control the facing. Useful for visual sync.
    // However, we don't want them to be able to directly turn around if they're not in a ball.
    if (snolfEngine->currentShotState == SNOLF_SHOT_READY && !snolfEngine->isSpinShot && (entity->animator.animationID == ANI_JUMP || entity->animator.animationID == ANI_DROPDASH))
    {
        if (entity->left)
        {
            entity->direction = FLIP_X;
        }
        else if (entity->right)
        {
            entity->direction = FLIP_NONE;
        }
    }

    // Update things like the Snolf Shot Meter.
    Snolf_UpdateShotLogic(player, entity, snolfEngine);

    // Ensure that the player's animation speed is set correctly in Spin Shot mode.
    if (snolfEngine->isSpinShot)
    {
        entity->animator.speed = 30 + (abs(snolfEngine->spinPower * 30));
    }
}

void Snolf_UpdateShotLogic(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Has the player pressed the shot button?
    if (entity->jumpPress)
    {
        RSDK.PrintLog(PRINT_NORMAL, "Player hit jump.");
        Snolf_HandleButtonPress(player, entity, snolfEngine);
        return;
    }

    // Is the player holding the jump button, and we're in shot-ready state?
    if (entity->jumpHold)
    {
        if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
        {
            if (entity->state == Player_State_Air || entity->state == Player_State_Roll)
            {
                snolfEngine->shotResetPressTimer++;

                if (snolfEngine->shotResetPressTimer >= 90)
                {
                    Snolf_ResetShot(player, entity, snolfEngine);
                    return;
                }
            }
        }
    }
    else
    {
        // Player is not currently pressing anything; set the shot-reset press timer back to 0.
        snolfEngine->shotResetPressTimer = 0;
    }

    // Increment the accumulator, ensuring it wraps every 256.
    snolfEngine->shotTimer = (snolfEngine->shotTimer + 1) % 256;

    // No button press; let's do the normal shot logic.
    // First, are we currently teeing up a shot?
    if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
    {
        return; // If not, we skip.
    }

    // We're currently in shot mode.
    // Store the player's current position for future shot resetting.
    snolfEngine->lastGoodShotPosition = entity->position;

    // Let's get a tasty sine wave from our accumulator.
    int32 sine = RSDK.Sin256(snolfEngine->shotTimer);
    int32 cosine = RSDK.Cos256(snolfEngine->shotTimer);

    // Set the Horizontal shot speed.
    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        snolfEngine->horizShotPower = (sine);
    }

    // Set the Vertical shot speed; using a 0.0 to 1.0 cosine wave instead of -1.0 to 1.0, so that the meter only goes up.
    if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL)
    {
        snolfEngine->vertShotPower = ((cosine / 2) + 127);
    }

    // If the player has started a Spin Shot, do that logic in its own function.
    if (snolfEngine->currentShotState == SNOLF_SHOT_SET_SPIN)
    {
        Snolf_UpdateSpinSetLogic(player, entity, snolfEngine);
    }
}

void Snolf_UpdateSpinSetLogic(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Player is trying to set spin.

    // Handle button presses. Tapping right to increase power that way.
    // We check against the player's previous input state so that it doesn't just increase the meter every frame the button is held.
    if (entity->right && !snolfEngine->wasRight)
    {
        snolfEngine->spinPower++;

        if (snolfEngine->spinPower > 0)
        {
            entity->direction = FLIP_NONE;
        }

        if (snolfEngine->spinPower > 3)
        {
            snolfEngine->spinPower = 3;
        }

        RSDK.PlaySfx(snolfEngine->sfxLockHoriz, false, 255);
    }

    // Tapping left to increase power that way instead.
    if (entity->left && !snolfEngine->wasLeft)
    {
        snolfEngine->spinPower--;

        if (snolfEngine->spinPower < 0)
        {
            entity->direction = FLIP_X;
        }

        if (snolfEngine->spinPower < -3)
        {
            snolfEngine->spinPower = -3;
        }

        RSDK.PlaySfx(snolfEngine->sfxLockHoriz, false, 255);
    }

    // Ensure animation speed is up to date.
    entity->animator.speed = 30 + (abs(snolfEngine->spinPower * 30));

    // Set the previous input state.
    snolfEngine->wasLeft = entity->left;
    snolfEngine->wasRight = entity->right;
}

void Snolf_ResetShot(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Reset the press timer.
    snolfEngine->shotResetPressTimer = 0;

    // Set Snolf back to the last-known-good X and Y.
    entity->position = snolfEngine->lastGoodShotPosition;

    // Zero out velocities etc.
    entity->velocity.x = 0;
    entity->velocity.y = 0;
    entity->groundVel = 0;

    if (!entity->sidekick)
    {
        RSDK.PlaySfx(snolfEngine->sfxResetShot, false, 255);
    }
}

void Snolf_HandleButtonPress(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Is player's ground speed below threshold, or are we in force-allow mode?
    if ((abs(entity->groundVel) < 0x10000 || snolfEngine->forceAllow))
    {
        // Is the player currently using an air ability? If so, don't Snolf!
        if (entity->animator.animationID != ANI_JUMP && entity->animator.animationID != ANI_DROPDASH)
        {
            return;
        }

        // Not Snolfing yet; let's begin a new Snolf shot.
        if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
        {
            snolfEngine->horizShotPower = 0;
            snolfEngine->vertShotPower = 0;
            snolfEngine->shotTimer = 0;
            snolfEngine->isSpinShot = false;

            // Player is holding down, and is Sonic... set that spin!
            if (entity->down && entity->characterID == ID_SONIC)
            {
                snolfEngine->currentShotState = SNOLF_SHOT_SET_SPIN;
                snolfEngine->isSpinShot = true;
                snolfEngine->spinPower = 0;
                if (!entity->sidekick)
                {
                    RSDK.PlaySfx(snolfEngine->sfxAdjustSpin, false, 255);
                }

                RSDK.SetSpriteAnimation(entity->aniFrames, ANI_DROPDASH, &entity->animator, true, 0);
                return;
            }

            snolfEngine->currentShotState = SNOLF_SHOT_HORIZONTAL;

            // If player is facing left, adjust the accumulator so that it's partially through its cycle, so that the horiztonal shot starts moving left.
            if ((entity->direction & FLIP_X) != 0 || entity->left)
            {
                snolfEngine->shotTimer = 127;
            }
            if (entity->right)
            {
                snolfEngine->shotTimer = 0;
            }

            if (!entity->sidekick)
            {
                RSDK.PlaySfx(snolfEngine->sfxStartSnolf, false, 255);
            }

            RSDK.PrintLog(PRINT_NORMAL, "Starting a Snolf!");
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL) // Horizontal Shot locked in - let's move to vertical!
        {
            // [SNOLF TODO] Play SFX.
            if (!entity->sidekick)
            {
                RSDK.PlaySfx(snolfEngine->sfxLockHoriz, false, 255);
            }
            RSDK.PrintLog(PRINT_NORMAL, "Horizontal strength locked in at %d!", snolfEngine->horizShotPower);
            snolfEngine->shotTimer = 127; // Start partway through the cycle.
            snolfEngine->currentShotState = SNOLF_SHOT_VERTICAL;
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL) // Vertical Shot locked in - Snolf that ball!!
        {
            RSDK.PrintLog(PRINT_NORMAL, "Vertical strength locked in at %d!", snolfEngine->vertShotPower);

            if (!entity->sidekick)
            {
                if (!snolfEngine->isSpinShot)
                {
                    RSDK.SetSpriteAnimation(entity->aniFrames, ANI_JUMP, &entity->animator, true, 0);
                    RSDK.PlaySfx(snolfEngine->sfxLaunchSnolf, false, 255);
                }
                else
                {
                    RSDK.PlaySfx(snolfEngine->sfxLaunchSpinSnolf, false, 255);
                }
            }

            // Reset Snolfing State.
            snolfEngine->currentShotState = SNOLF_SHOT_READY;

            // Force the player into the air.
            entity->state = Player_State_Air;
            entity->onGround = false;

            // Ptchoo!
            entity->velocity.x = TO_FIXED(snolfEngine->horizShotPower) / 18;
            entity->velocity.y = TO_FIXED(0 - snolfEngine->vertShotPower) / 18;

            // Force groundVel to a non-zero number; this helps inform the physics engine.
            entity->groundVel = (entity->velocity.x < 0) ? -TO_FIXED(4) : TO_FIXED(4);
            entity->applyJumpCap = false;

            // Track the number of shots the player has taken.
            snolfEngine->shotsTaken++;
            backupShotsTaken = snolfEngine->shotsTaken; // This is so that when you respawn, it doesn't get set back to 0.

            // Allow non-Sonic characters to use their air abilities.
            if (entity->characterID != ID_SONIC)
            {
                entity->jumpAbilityState = 1;
            }
            else
            {
                // Allow Sonic to use an air ability IF he has a shield equipped.
                if (entity->shield == SHIELD_BUBBLE || entity->shield == SHIELD_FIRE || entity->shield == SHIELD_LIGHTNING)
                {
                    entity->jumpAbilityState = 1;
                }
            }

            RSDK.PrintLog(PRINT_NORMAL, "Successful Snolf!");
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_SET_SPIN) // Lock in spin shot.
        {
            snolfEngine->currentShotState = SNOLF_SHOT_HORIZONTAL;
            snolfEngine->shotTimer = 0;

            if (snolfEngine->spinPower == 0)
            {
                snolfEngine->isSpinShot = false;
                RSDK.SetSpriteAnimation(entity->aniFrames, ANI_JUMP, &entity->animator, true, 0);
            }
        }
    }
}

void Snolf_Draw(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Is this player a CPU follower? Don't show their UI if so.
    if (entity->sidekick)
    {
        return;
    }

    // Draw "Shots" text and count.
    Vector2 shotsDrawPos;
    shotsDrawPos.x = TO_FIXED(16);
    shotsDrawPos.y = TO_FIXED(60);
    RSDK.DrawSprite(&snolfEngine->shotsTextAnimator, &shotsDrawPos, true);
    shotsDrawPos.x = shotsDrawPos.x + TO_FIXED(97);
    shotsDrawPos.y = shotsDrawPos.y + TO_FIXED(14);
    HUD_DrawNumbersBase10(&shotsDrawPos, snolfEngine->shotsTaken, 0);

    // Draw Meters
    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        Vector2 barDrawPos;
        barDrawPos.x = entity->position.x - TO_FIXED(32);
        barDrawPos.y = entity->position.y - TO_FIXED(16);
        RSDK.DrawSprite(&snolfEngine->horizBarAnimator, &barDrawPos, false);

        float vectorBarLength = ((float)snolfEngine->horizShotPower / 256.0f) * 30.0f;
        int32 vectorBarLen = (int)vectorBarLength;

        if (snolfEngine->horizShotPower >= 0)
        {
            RSDK.DrawRect(entity->position.x, entity->position.y - TO_FIXED(15), TO_FIXED(vectorBarLen), TO_FIXED(8), 0xF0F000, 0xEF, INK_ALPHA, false);
        }
        else
        {
            RSDK.DrawRect(entity->position.x + TO_FIXED(vectorBarLen), entity->position.y - TO_FIXED(15), TO_FIXED(-vectorBarLen), TO_FIXED(8), 0xF0F000, 0xEF, INK_ALPHA, false);
        }
    }

    if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL)
    {
        Vector2 barDrawPos;
        barDrawPos.x = entity->position.x - TO_FIXED(6);
        barDrawPos.y = entity->position.y - TO_FIXED(60);

        float vectorBarLength = ((float)snolfEngine->vertShotPower / 256.0f) * 40.0f;
        int32 vectorBarLen = (int)vectorBarLength;

        RSDK.DrawSprite(&snolfEngine->vertBarAnimator, &barDrawPos, false);
        RSDK.DrawRect(entity->position.x - TO_FIXED(5), entity->position.y - TO_FIXED(18) - TO_FIXED(vectorBarLen), TO_FIXED(9), TO_FIXED(vectorBarLen), 0xF0F000, 0xEF, INK_ALPHA, false);
    }

    if (snolfEngine->currentShotState == SNOLF_SHOT_SET_SPIN)
    {
        Vector2 drawChevronPos;
        drawChevronPos.x = entity->position.x - TO_FIXED(4);
        drawChevronPos.y = entity->position.y - TO_FIXED(16);
        snolfEngine->chevRAnimator.frameID = 0;
        snolfEngine->chevLAnimator.frameID = 0;

        if (snolfEngine->spinPower > 0)
        {
            snolfEngine->chevRAnimator.frameID = abs(snolfEngine->spinPower);
        }
        if (snolfEngine->spinPower < 0)
        {
            snolfEngine->chevLAnimator.frameID = abs(snolfEngine->spinPower);
        }

        drawChevronPos.x += TO_FIXED(12);
        RSDK.DrawSprite(&snolfEngine->chevRAnimator, &drawChevronPos, false);
        drawChevronPos.x -= TO_FIXED(24);
        RSDK.DrawSprite(&snolfEngine->chevLAnimator, &drawChevronPos, false);
    }
}
