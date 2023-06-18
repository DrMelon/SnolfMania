#ifndef OBJ_DASHLIFT_H
#define OBJ_DASHLIFT_H

#include "GameAPI/Game.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 sfxPulley;
} ObjectDashLift ;

// Entity Class
typedef struct {
    RSDK_ENTITY
    // Platform Base
    StateMachine(state);
    StateMachine(stateCollide);
    int32 type;
    Vector2 amplitude;
    int32 length;
    bool32 hasTension;
    int8 frameID;
    uint8 collision;
    Vector2 tileOrigin;
    Vector2 centerPos;
    Vector2 drawPos;
    Vector2 collisionOffset;
    int32 stood;
    int32 timer;
    int32 stoodAngle;
    uint8 stoodPlayers;
    uint8 pushPlayersL;
    uint8 pushPlayersR;
    Hitbox hitbox;
    Animator animator;
    int32 childCount;
    // Dash Lift
    int32 startOff;
    uint8 activePlayers;
} EntityDashLift ;

// Object Struct
extern ObjectDashLift *DashLift;

extern void (*DashLift_CheckOffScreen)();
// Standard Entity Events
bool32 DashLift_State_HandleDash_Snolfed(bool32 skipped);

#endif //! OBJ_DASHLIFT_H
