#ifndef MOD_CONFIG_H
#define MOD_CONFIG_H

#include "GameAPI/Game.h"

typedef struct
{
    // [SNOLF TODO] Options and settings for the mod.
    bool32 enableNewMeterStyle;

} ModConfig;

extern ModConfig config;

#endif // !MOD_CONFIG_H
