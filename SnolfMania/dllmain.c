#include "../GameAPI/C/GameAPI/Game.h"
#include "Objects/Player.h"
#include "ModConfig.h"

// [SNOLF TODO] Includes

ModConfig config;

#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);

#define ADD_PUBLIC_FUNC(func) Mod.AddPublicFunction(#func, (void *)(func))
#endif

StateMachine(Player_State_Ground);
StateMachine(Player_State_Roll);
StateMachine(Player_State_Air);

void InitModAPI(void)
{

    // [SNOLF TODO] Load any persistent settings into the ModConfig.
    config.enableNewMeterStyle = Mod.GetSettingsBool("", "Config:enableNewSnolfMeterStyle", true);
    Mod.SetSettingsBool("Config:enableNewSnolfMeterStyle", config.enableNewMeterStyle);
    Mod.SaveSettings();

    // [SNOLF TODO] Register any state hooks
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "Player_State_Death"), Snolf_EnsureInfiniteLives, false);

    // [SNOLF TODO] Get existing public functions.
    // e.g, Player_JumpAbility_Sonic = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Sonic");
    Player_State_Roll = Mod.GetPublicFunction(NULL, "Player_State_Roll");
    Player_State_Ground = Mod.GetPublicFunction(NULL, "Player_State_Ground");
    Player_State_Air = Mod.GetPublicFunction(NULL, "Player_State_Air");

    // [SNOLF TODO] Register new functions with ADD_PUBLIC_FUNC.
    // Something like...
    // ADD_PUBLIC_FUNC(Snolf_UpdateShotStrength);

    // [SNOLF TODO] Use macros to inject custom code and overload functions from base game.
    MOD_REGISTER_OBJECT_HOOK(Player);
    MOD_REGISTER_OBJ_OVERLOAD(Player, Player_Update, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

#if RETRO_USE_MOD_LOADER
void InitModAPI(void);

bool32 LinkModLogic(EngineInfo *info, const char *id)
{
#if MANIA_USE_PLUS
    LinkGameLogicDLL(info);
#else
    LinkGameLogicDLL(*info);
#endif

    globals = Mod.GetGlobals();

    modID = id;

    InitModAPI();

    return true;
}
#endif
