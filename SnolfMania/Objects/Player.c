#include "GameAPI/Game.h"
#include "GameAPI/GameLink.h"

#include "Player.h"
#include "Snolf.h"
#include "SnolfEngine.h"
#include "../ModConfig.h"

ObjectPlayer *Player;

// Player update function. Runs every frame.
void Player_Update()
{
    RSDK_THIS(Player);

    Mod.Super(Player->classID, SUPER_UPDATE, NULL);

    // Perform main Snolf logic.
    Snolf_Main(Player, self, &self->snolfEngine);
}

// Player creation function. Runs once.
void Player_Create(void *data)
{
    RSDK_THIS(Player);

    Mod.Super(self->classID, SUPER_CREATE, NULL);
}

// In SNOLF, the Player has infinite lives.
void Snolf_EnsureInfiniteLives(void)
{
    RSDK_THIS(Player);

    // Force lives to be at least 4 (which becomes 3 after death code finishes). Kinda weird to do it this way, but it's cleaner than modifying the whole death function.
    if (self->lives < 4)
    {
        RSDK.PrintLog(PRINT_NORMAL, "Setting lives to 4.");
        self->lives = 4;
    }
}