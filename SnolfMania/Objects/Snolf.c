#include "GameAPI/Game.h"
#include "Player.h"
#include "HUD.h"
#include "Snolf.h"

void Snolf_Main(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Bail if controls are locked (e.g, cutscene)
    if (entity->controlLock)
    {
        return;
    }

    // Force player to be a ball.
    if (entity->state == Player_State_Ground)
    {
        Player_Action_Roll();
    }

    // Allow the player to control the facing. Useful for visual sync.
    if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
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

    Snolf_UpdateShotLogic(player, entity, snolfEngine);
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
            snolfEngine->shotResetPressTimer++;

            if (snolfEngine->shotResetPressTimer >= 90)
            {
                Snolf_ResetShot(player, entity, snolfEngine);
                return;
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

    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        snolfEngine->horizShotPower = (sine);
    }

    if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL)
    {
        snolfEngine->vertShotPower = ((cosine / 2) + 127);
    }
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

    // [SNOLF TODO] Play SFX
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
        // Not Snolfing yet; let's begin a new Snolf shot.
        if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
        {
            snolfEngine->horizShotPower = 0;
            snolfEngine->vertShotPower = 0;
            snolfEngine->shotTimer = 0;
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

            // [SNOLF TODO] Spawn UI shot meter elements.
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

            // [SNOLF TODO] Reset UI elements for vertical mode.
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL) // Vertical Shot locked in - Snolf that ball!!
        {
            RSDK.PrintLog(PRINT_NORMAL, "Vertical strength locked in at %d!", snolfEngine->vertShotPower);

            // [SNOLF TODO] Play SFX.
            if (!entity->sidekick)
            {
                RSDK.PlaySfx(snolfEngine->sfxLaunchSnolf, false, 255);
            }
            snolfEngine->currentShotState = SNOLF_SHOT_READY;

            // Force the player into the air.
            entity->state = Player_State_Air;
            entity->onGround = false;

            // Ptchoo!
            entity->velocity.x = TO_FIXED(snolfEngine->horizShotPower) / 18;
            entity->velocity.y = TO_FIXED(0 - snolfEngine->vertShotPower) / 20;
            // Force groundVel to a non-zero number; this helps inform the physics engine.
            entity->groundVel = (entity->velocity.x < 0) ? -TO_FIXED(4) : TO_FIXED(4);
            entity->applyJumpCap = false;

            // [SNOLF TODO] Hide UI elements.

            snolfEngine->shotsTaken++;
            RSDK.PrintLog(PRINT_NORMAL, "Successful Snolf!");
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

        float vectorBarLength = ((float)snolfEngine->vertShotPower / 256.0f) * 38.0f;
        int32 vectorBarLen = (int)vectorBarLength;

        RSDK.DrawSprite(&snolfEngine->vertBarAnimator, &barDrawPos, false);
        RSDK.DrawRect(entity->position.x - TO_FIXED(5), entity->position.y - TO_FIXED(18) - TO_FIXED(vectorBarLen), TO_FIXED(9), TO_FIXED(vectorBarLen), 0xF0F000, 0xEF, INK_ALPHA, false);
    }
}
