#include "../GameAPI/C/GameAPI/Game.h"
#include "Objects/Player.h"
#include "Objects/HUD.h"
#include "Objects/Zone.h"
#include "Objects/Starpost.h"
#include "Objects/DashLift.h"
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
void (*HUD_DrawNumbersBase10)(Vector2 *drawPos, int32 value, int32 digitCount) = NULL;
void (*Player_Input_P1)();
void (*Player_Input_P2_Player)();
void (*DashLift_CheckOffScreen)();

StateMachine(Player_State_Ground);
StateMachine(Player_State_Roll);
StateMachine(Player_State_Air);
StateMachine(Player_State_Crouch);
StateMachine(Player_State_LookUp);
StateMachine(Player_State_TubeRoll);
StateMachine(Player_State_Static);

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
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "Player_State_Air"), Player_State_Air_Snolfed, true);
    Mod.RegisterStateHook(Mod.GetPublicFunction(NULL, "DashLift_State_HandleDash"), DashLift_State_HandleDash_Snolfed, true);

    // [SNOLF TODO] Get existing public functions.
    // e.g, Player_JumpAbility_Sonic = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Sonic");
    Player_State_Roll = Mod.GetPublicFunction(NULL, "Player_State_Roll");
    Player_State_Ground = Mod.GetPublicFunction(NULL, "Player_State_Ground");
    Player_State_Air = Mod.GetPublicFunction(NULL, "Player_State_Air");
    Player_State_Crouch = Mod.GetPublicFunction(NULL, "Player_State_Crouch");
    Player_State_LookUp = Mod.GetPublicFunction(NULL, "Player_State_LookUp");
    Player_State_TubeRoll = Mod.GetPublicFunction(NULL, "Player_State_TubeRoll");
    Player_State_Static = Mod.GetPublicFunction(NULL, "Player_State_Static");

    Player_Action_Jump = Mod.GetPublicFunction(NULL, "Player_Action_Jump");
    Player_Action_Roll = Mod.GetPublicFunction(NULL, "Player_Action_Roll");
    Player_HandleAirMovement = Mod.GetPublicFunction(NULL, "Player_HandleAirMovement");
    Player_HandleGroundRotation = Mod.GetPublicFunction(NULL, "Player_HandleGroundRotation");
    Player_HandleGroundMovement = Mod.GetPublicFunction(NULL, "Player_HandleGroundMovement");
    Player_Input_P1 = Mod.GetPublicFunction(NULL, "Player_Input_P1");
    Player_Input_P2_Player = Mod.GetPublicFunction(NULL, "Player_Input_P2_Player");

    HUD_DrawNumbersBase10 = Mod.GetPublicFunction(NULL, "HUD_DrawNumbersBase10");

    Dust_State_DustTrail = Mod.GetPublicFunction(NULL, "Dust_State_DustTrail");
    DashLift_CheckOffScreen = Mod.GetPublicFunction(NULL, "DashLift_CheckOffScreen");

    // [SNOLF TODO] Register new functions with ADD_PUBLIC_FUNC.
    ADD_PUBLIC_FUNC(Player_State_Ground_Snolfed);
    ADD_PUBLIC_FUNC(Player_State_Roll_Snolfed);
    ADD_PUBLIC_FUNC(Player_HandleRollDeceleration_Snolfed);

    ADD_PUBLIC_FUNC(Player_HandleAirFriction_Snolfed);

    // [SNOLF TODO] Use macros to inject custom code and overload functions from base game.
    MOD_REGISTER_OBJECT_HOOK(Player);
    MOD_REGISTER_OBJECT_HOOK(Zone);
    MOD_REGISTER_OBJECT_HOOK(StarPost);
    MOD_REGISTER_OBJ_OVERLOAD(Player, Player_Update, NULL, NULL, Player_Draw, Player_Create, Player_StageLoad, NULL, NULL, NULL);
    MOD_REGISTER_OBJ_OVERLOAD(HUD, NULL, NULL, NULL, HUD_Draw, NULL, NULL, NULL, NULL, NULL);
}

#if RETRO_USE_MOD_LOADER
void InitModAPI(void);

bool32 LinkModLogic(EngineInfo *info, const char *id)
{
#if MANIA_USE_PLUS
    LinkGameLogicDLL(info);
#else
    LinkGameLogicDLL(info);
#endif

    globals = Mod.GetGlobals();

    modID = id;

    InitModAPI();

    return true;
}
#endif
