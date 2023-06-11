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
        // entity->state = Player_State_Roll; // [SNOLF TODO] - Prevent Roll state from being able to unroll player. Custom roll state required? Likely. We don't want them to be able to jump normally either.
        RSDK.SetSpriteAnimation(entity->aniFrames, ANI_JUMP, &entity->animator, false, 0);

        entity->pushing = 0;
        entity->state = Player_State_Roll;
        if (entity->collisionMode == CMODE_FLOOR)
            entity->position.y += entity->jumpOffset;
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

    if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL)
    {
        // [SNOLF TODO] Apply the sine wave to the shot.
    }

    // Debug hack
    snolfEngine->horizShotPower = 512;
    snolfEngine->vertShotPower = 256;
}

void Snolf_HandleButtonPress(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine)
{
    // Is player's ground speed below threshold, or are we in force-allow mode?
    // if ((entity->groundVel <= TO_FIXED(64) || snolfEngine->forceAllow) && entity->onGround)
    //{
    // Not Snolfing yet; let's begin a new Snolf shot.
    if (snolfEngine->currentShotState == SNOLF_SHOT_READY)
    {
        snolfEngine->horizShotPower = 0;
        snolfEngine->vertShotPower = 0;
        snolfEngine->shotTimer = 0;
        snolfEngine->currentShotState = SNOLF_SHOT_HORIZONTAL;

        // If player is facing left, adjust the accumulator so that it's partially through its cycle, so that the horiztonal shot starts moving left.
        if ((entity->direction & FLIP_X) != 0)
        {
            snolfEngine->shotTimer = 256;
        }

        // [SNOLF TODO] Spawn UI elements.
        RSDK.PrintLog(PRINT_NORMAL, "Starting a Snolf!");
    }
    else if (snolfEngine->currentShotState == SNOLF_SHOT_HORIZONTAL) // Horizontal Shot locked in - let's move to vertical!
    {
        // [SNOLF TODO] Play SFX.
        RSDK.PrintLog(PRINT_NORMAL, "Horizontal strength locked in at %d!", snolfEngine->horizShotPower);
        snolfEngine->shotTimer = 0;
        snolfEngine->currentShotState = SNOLF_SHOT_VERTICAL;
    }
    else if (snolfEngine->currentShotState == SNOLF_SHOT_VERTICAL) // Vertical Shot locked in - Snolf that ball!!
    {
        RSDK.PrintLog(PRINT_NORMAL, "Vertical strength locked in at %d!", snolfEngine->vertShotPower);

        // [SNOLF TODO] Play SFX.
        snolfEngine->currentShotState = SNOLF_SHOT_READY;

        // Force the player into the air.
        entity->state = Player_State_Air;

        // Ptchoo!
        entity->velocity.x = TO_FIXED(snolfEngine->horizShotPower / 20);
        entity->velocity.y = TO_FIXED((0 - snolfEngine->vertShotPower) / 20);
        entity->groundVel = 400;

        RSDK.PrintLog(PRINT_NORMAL, "Successful Snolf!");
    }
    //}
}