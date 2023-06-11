#ifndef SNOLF_H
#define SNOLF_H

#include "GameAPI/Game.h"
#include "Player.h"
#include "SnolfEngine.h"

// Main entry point for Snolf logic.
void Snolf_Main(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine);

// This function responds to the player pressing a button in the ready state for Snolfing.
void Snolf_HandleButtonPress(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine);

// This function controls the shot logic; that is, allowing the player to set the shot strength horizontally and vertically,
// and then finally launching the player.
void Snolf_UpdateShotLogic(ObjectPlayer *player, EntityPlayer *entity, SnolfEngine *snolfEngine);

#endif // SNOLF_H