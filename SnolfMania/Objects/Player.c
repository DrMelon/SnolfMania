#include "GameAPI/Game.h"
#include "GameAPI/GameLink.h"
#include "Player.h"
#include "../ModConfig.h"

ObjectPlayer *Player;

void Player_Update()
{
    RSDK_THIS(Player);

    // [SNOLF TODO] Insert update logic here.

    Mod.Super(Player->classID, SUPER_UPDATE, NULL);
}

// In SNOLF, the Player has infinite lives.
void Snolf_EnsureInfiniteLives(void)
{
    RSDK_THIS(Player);

    // Force lives to 99. Kinda weird to do it this way, but it's cleaner than modifying the whole death function.
    self->lives = 99;
}