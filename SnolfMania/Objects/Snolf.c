#include "GameAPI/Game.h"
#include "Player.h"
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

    // Allow the player to control the facing at any time. Useful for visual sync.
    if (entity->left)
    {
        entity->direction = FLIP_X;
    }
    else if (entity->right)
    {
        entity->direction = FLIP_NONE;
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

    // Player is not currently pressing anything; set the shot-reset press timer back to 0.
    snolfEngine->shotResetPressTimer = 0;

    // Increment the accumulator, ensuring it wraps every 1024.
    snolfEngine->shotTimer = (snolfEngine->shotTimer + 1) % 1024;

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
    int32 sine = RSDK.Sin1024(snolfEngine->shotTimer);
    int32 cosine = RSDK.Cos1024(snolfEngine->shotTimer);

    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        snolfEngine->horizShotPower = (sine / 20);
    }

    if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL)
    {
        snolfEngine->vertShotPower = ((cosine / 2) + 512) / 20;
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
                snolfEngine->shotTimer = 512;
            }
            if (entity->right)
            {
                snolfEngine->shotTimer = 0;
            }

            // [SNOLF TODO] Spawn UI shot meter elements.
            RSDK.PrintLog(PRINT_NORMAL, "Starting a Snolf!");
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL) // Horizontal Shot locked in - let's move to vertical!
        {
            // [SNOLF TODO] Play SFX.
            RSDK.PrintLog(PRINT_NORMAL, "Horizontal strength locked in at %d!", snolfEngine->horizShotPower);
            snolfEngine->shotTimer = 512; // Start partway through the cycle.
            snolfEngine->currentShotState = SNOLF_SHOT_VERTICAL;

            // [SNOLF TODO] Reset UI elements for vertical mode.
        }
        else if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL) // Vertical Shot locked in - Snolf that ball!!
        {
            RSDK.PrintLog(PRINT_NORMAL, "Vertical strength locked in at %d!", snolfEngine->vertShotPower);

            // [SNOLF TODO] Play SFX.
            snolfEngine->currentShotState = SNOLF_SHOT_READY;

            // Force the player into the air.
            entity->state = Player_State_Air;
            entity->onGround = false;

            // Ptchoo!
            entity->velocity.x = TO_FIXED(snolfEngine->horizShotPower);
            entity->velocity.y = TO_FIXED(0 - snolfEngine->vertShotPower);
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
    shotsDrawPos.y = TO_FIXED(64);
    RSDK.DrawSprite(&snolfEngine->shotsTextAnimator, &shotsDrawPos, true);

    // Draw Meters
    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        RSDK.DrawLine(entity->position.x, entity->position.y, entity->position.x + TO_FIXED(snolfEngine->horizShotPower), entity->position.y, 0x00FF00, 0x7F, INK_ADD, false);
    }

    if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL)
    {
        RSDK.DrawLine(entity->position.x, entity->position.y, entity->position.x + TO_FIXED(snolfEngine->horizShotPower), entity->position.y, 0xFF00FF, 0x7F, INK_ADD, false);
        RSDK.DrawLine(entity->position.x, entity->position.y, entity->position.x, entity->position.y + TO_FIXED(0 - snolfEngine->vertShotPower), 0x00FF00, 0x7F, INK_ADD, false);
    }
}
