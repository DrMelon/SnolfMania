#ifndef SNOLF_ENGINE_H
#define SNOLF_ENGINE_H

#include "GameAPI/Game.h"

// This enum represents the possible states the "Snolf Engine" can be in.
typedef enum
{
    SNOLF_SHOT_READY,
    SNOLF_SHOT_HORIZONTAL,
    SNOLF_SHOT_VERTICAL
} SnolfShotState;

// This is the "Snolf Engine" - it is an abstraction that contains a bunch of useful data for performing Snolf functions.
// Each player object should own or create one of these, so that local-multiplayer Snolfing is supported.
typedef struct
{
    SnolfShotState currentShotState;
    uint32 shotTimer;
    uint32 shotResetPressTimer;

    int32 horizShotPower;
    int32 vertShotPower;

    bool32 isSpinShot;
    int32 spinPower;

    Vector2 lastGoodShotPosition;

    bool32 forceAllow;

    int32 shotsTaken;
    int32 shotsTakenTotal;

    uint16 shotsFrames;
    Animator shotsTextAnimator;
    Animator horizBarAnimator;
    Animator vertBarAnimator;

    uint16 sfxStartSnolf;
    uint16 sfxLockHoriz;
    uint16 sfxLaunchSnolf;
    uint16 sfxAdjustSpin;
    uint16 sfxLaunchSpinSnolf;
    uint16 sfxResetShot;

} SnolfEngine;

#endif // SNOLF_ENGINE_H