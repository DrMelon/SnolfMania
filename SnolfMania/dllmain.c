#include "../GameAPI/C/GameAPI/Game.h"
#include "ModConfig.h"

// [SNOLF TODO] Includes

ModConfig config;

#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);

#define ADD_PUBLIC_FUNC(func) Mod.AddPublicFunction(#func, (void *)(func))
#endif

void InitModAPI(void)
{
    // [SNOLF TODO] Load any persistent settings into the ModConfig. 
    
    // [SNOLF TODO] Register any state hooks 
    // e.g, Mod.AddModCallback(MODCB_ONSTAGELOAD, Snolf_ResetTimersAndShotPositions);

    // [SNOLF TODO] Get existing public functions. 
    // e.g, Player_JumpAbility_Sonic = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Sonic");
    
    // [SNOLF TODO] Register new functions with ADD_PUBLIC_FUNC. 
    // Something like...
    // ADD_PUBLIC_FUNC(Snolf_UpdateShotStrength);

    // [SNOLF TODO] Use macros to inject custom code and overload functions from base game. 
    // Something like...
    // MOD_REGISTER_OBJECT_HOOK(Player);
    // MOD_REGISTER_OBJ_OVERLOAD(Player, Player_Update, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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
