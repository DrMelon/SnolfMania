#include "DashLift.h"
#include "Player.h"
#include "Zone.h"

ObjectDashLift* DashLift;

bool32 DashLift_State_HandleDash_Snolfed(bool32 skipped)
{
    RSDK_THIS(DashLift);

    int32 totalSpeed = 0;
    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntitySlot(player);

        if (!((1 << playerID) & self->stoodPlayers)) {
            if (((1 << playerID) & self->activePlayers)) {
                player->position.x = self->position.x;
                self->activePlayers &= ~(1 << playerID);
                if (player->animator.animationID == ANI_JUMP)
                    player->velocity.x = 0;
            }
        }
        else {
            int32 anim = player->animator.animationID;
            // Need to modify the condition of how the player accesses this DashLift,.
            if (anim == ANI_JUMP || (anim == ANI_JUMP && ((1 << playerID) & self->activePlayers))) {
                self->activePlayers |= (1 << playerID);

                
                player->velocity.x -= player->velocity.x >> 5;
                player->position.x = self->position.x;
                if (player->direction == FLIP_X) {
                    if ((player->sidekick && totalSpeed < 0) || self->drawPos.y >= self->amplitude.y)
                        continue;

                    int32 speed = 0;
                    //if (player->state == Player_State_Spindash) {
                    //    if (player->superState == SUPERSTATE_SUPER)
                    //        speed = ((player->abilityTimer >> 1) & 0x7FFF8000) + 0xB0000;
                    //    else
                    //        speed = ((player->abilityTimer >> 1) & 0x7FFF8000) + 0x80000;
                    //}
                    //else {
                        speed = -player->groundVel * 2;
                    //}

                    speed -= 0x40000;
                    if (speed < 0) {
                        speed              = 0;
                        player->groundVel  = 0;
                        player->velocity.x = 0;
                    }

                    self->drawPos.y += speed >> 2;
                    totalSpeed += speed >> 2;
                    self->animator.timer += abs((speed >> 2) >> 16);

                    if (self->animator.timer >= 4) {
                        self->animator.timer = 0;
                        if (++self->animator.frameID > 4)
                            self->animator.frameID = 0;
                    }

                    //if (!(Zone->timer & 0xF))
                        //RSDK.PlaySfx(DashLift->sfxPulley, false, 255);
                }
                else {
                    if ((player->sidekick && totalSpeed > 0) || self->drawPos.y <= self->amplitude.x)
                        continue;

                    int32 speed = 0;
                    //if (player->state == Player_State_Spindash) {
                    //    if (player->superState == SUPERSTATE_SUPER)
                    //        speed = -((player->abilityTimer >> 1) & 0x7FFF8000) - 0xB0000;
                    //    else
                    //        speed = -((player->abilityTimer >> 1) & 0x7FFF8000) - 0x80000;
                    //}
                    //else {
                        speed = -player->groundVel * 2;
                    //}

                    speed += 0x40000;
                    if (speed > 0) {
                        speed              = 0;
                        player->groundVel  = 0;
                        player->velocity.x = 0;
                    }

                    self->drawPos.y += speed >> 2;
                    totalSpeed += speed >> 2;
                    self->animator.timer -= abs((speed >> 2) >> 16);

                    if (self->animator.timer <= 0) {
                        self->animator.timer = 3;
                        if (--self->animator.frameID < 0)
                            self->animator.frameID = 4;
                    }

                    //if (!(Zone->timer & 0xF))
                        //RSDK.PlaySfx(DashLift->sfxPulley, false, 255);
                }
            }
            else {
                self->activePlayers &= ~(1 << playerID);
            }
        }
    }

    // Segfaulting again. Let's see about inlining this
    DashLift_CheckOffScreen();

    return true; // Skip original dashlift logic.
}

