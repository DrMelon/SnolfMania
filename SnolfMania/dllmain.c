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

    // [SNOLF TODO] Use macros to inject custom code and overload functions from base game. 
    // e.g, something like MOD_REGISTER_OBJ_OVERLOAD(...SNOLF_Player_Update...Player_Update...)
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
