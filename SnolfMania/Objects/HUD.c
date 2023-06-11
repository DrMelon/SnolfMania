#include "GameAPI/Game.h"
#include "GameAPI/GameLink.h"
#include "HUD.h"
#include "Snolf.h"
#include "SnolfEngine.h"
#include "../ModConfig.h"

ObjectHUD *HUD;

void HUD_Draw()
{
    RSDK_THIS(HUD);

    Mod.Super(HUD->classID, SUPER_DRAW, NULL);

    EntityPlayer *entity = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    Snolf_Draw(Player, entity, &entity->snolfEngine);
}