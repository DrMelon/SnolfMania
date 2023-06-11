#include "../GameAPI/C/GameAPI/Game.h"
#include "Objects/Player.h"
#include "ModConfig.h"

// [SNOLF TODO] Includes

ModConfig config;

#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);

#define ADD_PUBLIC_FUNC(func) Mod.AddPublicFunction(#func, (void *)(func))
#endif

// Resolve Externals
void (*Player_Action_Jump)(EntityPlayer *entity);
void (*Player_Action_Roll)();
void (*Player_HandleAirMovement)();
void (*Player_HandleGroundRotation)();
void (*Player_HandleGroundMovement)();
// void (*Player_HandleGroundAnimation)();
// void (*Player_Gravity_False)();

StateMachine(Player_State_Ground);
StateMachine(Player_State_Roll);
StateMachine(Player_State_Air);
StateMachine(Player_State_Crouch);
StateMachine(Player_State_LookUp);
StateMachine(Player_State_TubeRoll);

StateMachine(Dust_State_DustTrail);

void InitModAPI(void)
{

    // [SNOLF TODO] Load any persistent settings into the ModConfig.
    config.enableNewMeterStyle = Mod.GetSettingsBool("", "Config:enableNewSnolfMeterStyle", true);
    Mod.SetSettingsBool("Config:enableNewSnolfMeterStyle", config.enableNewMeterStyle);
    Mod.SaveSettings();

    // [SNOLF TODO] Register any state hooks
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "Player_State_Death"), Snolf_EnsureInfiniteLives, false);
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "Player_State_Ground"), Player_State_Ground_Snolfed, true);
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "Player_State_Roll"), Player_State_Roll_Snolfed, true);

    // [SNOLF TODO] Get existing public functions.
    // e.g, Player_JumpAbility_Sonic = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Sonic");
    Player_State_Roll = Mod.GetPublicFunction(NULL, "Player_State_Roll");
    Player_State_Ground = Mod.GetPublicFunction(NULL, "Player_State_Ground");
    Player_State_Air = Mod.GetPublicFunction(NULL, "Player_State_Air");
    Player_State_Crouch = Mod.GetPublicFunction(NULL, "Player_State_Crouch");
    Player_State_LookUp = Mod.GetPublicFunction(NULL, "Player_State_LookUp");
    Player_State_TubeRoll = Mod.GetPublicFunction(NULL, "Player_State_TubeRoll");

    Player_Action_Jump = Mod.GetPublicFunction(NULL, "Player_Action_Jump");
    Player_Action_Roll = Mod.GetPublicFunction(NULL, "Player_Action_Roll");
    Player_HandleAirMovement = Mod.GetPublicFunction(NULL, "Player_HandleAirMovement");
    Player_HandleGroundRotation = Mod.GetPublicFunction(NULL, "Player_HandleGroundRotation");
    Player_HandleGroundMovement = Mod.GetPublicFunction(NULL, "Player_HandleGroundMovement");
    // Player_HandleGroundAnimation = Mod.GetPublicFunction(NULL, "Player_HandleGroundAnimation");
    // Player_Gravity_False = Mod.GetPublicFunction(NULL, "Player_Gravity_False");

    Dust_State_DustTrail = Mod.GetPublicFunction(NULL, "Dust_State_DustTrail");

    // [SNOLF TODO] Register new functions with ADD_PUBLIC_FUNC.
    ADD_PUBLIC_FUNC(Player_State_Ground_Snolfed);
    ADD_PUBLIC_FUNC(Player_State_Roll_Snolfed);
    ADD_PUBLIC_FUNC(Player_HandleRollDeceleration_Snolfed);

    // [SNOLF TODO] Use macros to inject custom code and overload functions from base game.
    MOD_REGISTER_OBJECT_HOOK(Player);
    MOD_REGISTER_OBJ_OVERLOAD(Player, Player_Update, NULL, NULL, Player_Draw, NULL, NULL, NULL, NULL, NULL);
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
