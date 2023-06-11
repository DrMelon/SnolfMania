#include "GameAPI/Game.h"
#include "GameAPI/GameLink.h"

#include "Player.h"
#include "Snolf.h"
#include "SnolfEngine.h"
#include "../ModConfig.h"

ObjectPlayer *Player;
EntityDust *Dust;

// Player update function. Runs every frame.
void Player_Update()
{
    RSDK_THIS(Player);

    Mod.Super(Player->classID, SUPER_UPDATE, NULL);

    // Perform main Snolf logic.
    Snolf_Main(Player, self, &self->snolfEngine);
}

// Player creation function. Runs once.
void Player_Create(void *data)
{
    RSDK_THIS(Player);

    Mod.Super(self->classID, SUPER_CREATE, NULL);

    self->snolfEngine.shotsFrames = RSDK.LoadSpriteAnimation("Global/SnolfHUD.bin", SCOPE_STAGE);
    RSDK.SetSpriteAnimation(self->snolfEngine.shotsFrames, 0, &self->snolfEngine.shotsTextAnimator, true, 0);
    self->snolfEngine.shotsTaken = 0;
}

// Player draw function. Runs on render.
void Player_Draw()
{
    RSDK_THIS(Player);

    Mod.Super(Player->classID, SUPER_DRAW, NULL);

    Snolf_Draw(Player, self, &self->snolfEngine);
}

void Player_StageLoad()
{
    RSDK_THIS(Player);

    Mod.Super(Player->classID, SUPER_STAGELOAD, NULL);
}

// Overriding the player's ground state; we want to prevent the player from taking actions like jumping or spindashing.
// However, we can't just take the ground-state code *out*; it needs to work when the player is doing things like interacting with cutscenes.
// So, we add a lot of checks to see if self->controlLock is set.
bool32 Player_State_Ground_Snolfed(bool32 skipped)
{
    RSDK_THIS(Player);
    if (!self->onGround)
    {
        self->state = Player_State_Air;

        Player_HandleAirMovement();
    }
    else
    {
        if (!self->groundVel)
        {
            if (self->left)
                --self->pushing;
            else if (self->right)
                ++self->pushing;
            else
                self->pushing = 0;
        }
        else
        {
            if (!self->left && !self->right)
            {
                self->pushing = 0;
            }
            else
            {
                if (abs(self->groundVel) > 0x10000)
                    self->pushing = 0;
            }
        }

        if (self->invertGravity)
        {
            if (self->collisionMode != CMODE_ROOF)
                self->pushing = 0;
        }
        else if (self->collisionMode != CMODE_FLOOR)
        {
            self->pushing = 0;
        }
        Player_HandleGroundRotation();
        Player_HandleGroundMovement();

        // Player_Gravity_False segfaulting for some reason. It's only 3 lines long, so i'll expand it here.
        // Player_Gravity_False();
        if (self->camera)
        {
            self->camera->disableYOffset = false;
        }
        self->jumpAbilityState = 0;

        self->nextAirState = Player_State_Air;

        // Player_HandleGroundAnimation();
        Player_HandleGroundAnimation_Snolfed();

        if (self->jumpPress && self->controlLock)
        {
            Player_Action_Jump(self);
            self->timer = 0;
        }
        else
        {
            // if (self->groundVel)
            //{
            int32 minRollVel = self->state == Player_State_Crouch ? 0x11000 : 0x8800;
            // if (abs(self->groundVel) >= minRollVel && !self->left && !self->right && self->down)
            //{
            Player_Action_Roll();
            RSDK.PlaySfx(Player->sfxRoll, false, 255);
            //}
            //}
            /*if (((self->angle < 0x20 || self->angle > 0xE0) && !self->collisionMode) || (self->invertGravity && self->angle == 0x80))
            {
                if (self->up)
                {
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_LOOK_UP, &self->animator, true, 1);
                    self->timer = 0;
                    self->state = Player_State_LookUp;
                }
                else if (self->down)
                {
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_CROUCH, &self->animator, true, 1);
                    self->timer = 0;
                    self->state = Player_State_Crouch;
                }
            }*/
        }
    }

    return true; // Skip original ground code.
}

// Overriding the player's roll state; we want to prevent the player from taking actions like jumping.
// However, we can't just take the ground-state code *out*; it needs to work when the player is doing things like interacting with cutscenes.
// So, we add a lot of checks to see if self->controlLock is set.
bool32 Player_State_Roll_Snolfed(bool32 skipped)
{
    RSDK_THIS(Player);
    Player_HandleGroundRotation();
    // Player_HandleRollDeceleration();
    Player_HandleRollDeceleration_Snolfed(); // We don't want to unroll when slowing down!

    self->applyJumpCap = false;
    if (!self->onGround)
    {
        self->state = Player_State_Air;
        Player_HandleAirMovement();
    }
    else
    {
        if (self->characterID == ID_TAILS)
            self->animator.speed = 120;
        else
            self->animator.speed = ((abs(self->groundVel) * 0xF0) / 0x60000) + 0x30;

        if (self->animator.speed > 0xF0)
            self->animator.speed = 0xF0;

        // Player_Gravity_False segfaulting for some reason. It's only 3 lines long, so i'll expand it here.
        // Player_Gravity_False();
        if (self->camera)
        {
            self->camera->disableYOffset = false;
        }
        self->jumpAbilityState = 0;

        if (self->jumpPress && self->controlLock)
            Player_Action_Jump(self);
    }

    return true; // Skip original ground code.
}

bool32 Player_State_Air_Snolfed(bool32 skipped)
{
    RSDK_THIS(Player);

#if GAME_VERSION != VER_100
    self->tileCollisions = TILECOLLISION_DOWN;
#endif
    Player_HandleAirFriction_Snolfed();

    if (self->onGround)
    {
        self->state = Player_State_Ground;

        // Player_Gravity_False segfaulting for some reason. It's only 3 lines long, so i'll expand it here.
        // Player_Gravity_False();
        if (self->camera)
        {
            self->camera->disableYOffset = false;
        }
        self->jumpAbilityState = 0;
    }
    else
    {
        Player_HandleAirMovement();
        self->nextGroundState = Player_State_Ground;

        if (self->velocity.y > 0)
        {
            if (self->animator.animationID >= ANI_SPRING_TWIRL)
            {
                if (self->animator.animationID <= ANI_SPRING_DIAGONAL)
                {
                    RSDK.SetSpriteAnimation(self->aniFrames, self->animationReserve, &self->animator, false, 0);
                }
                else if (self->animator.animationID == ANI_SPRING_CS && !self->animator.frameID)
                {
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_AIR_WALK, &self->animator, false, 0);
                }
            }
        }

        switch (self->animator.animationID)
        {
        case ANI_IDLE:
        case ANI_WALK:
            if (self->animator.speed < 64)
                self->animator.speed = 64;
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_AIR_WALK, &self->animator, false, self->animator.frameID);
            break;

        case ANI_LOOK_UP:
        case ANI_CROUCH:
        case ANI_SKID_TURN:
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_AIR_WALK, &self->animator, false, 0);
            break;

        case ANI_JOG:
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_AIR_WALK, &self->animator, false, 0);
            break;

        case ANI_JUMP:
            if (self->velocity.y >= self->jumpCap)
                StateMachine_Run(self->stateAbility);
            break;

        case ANI_SKID:
            if (self->skidding <= 0)
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_AIR_WALK, &self->animator, false, self->animator.frameID);
            else
                self->skidding--;
            break;

        case ANI_SPINDASH:
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
            break;

        default:
            break;
        }
    }

    return true; // Skip original air code.
}

void Player_HandleRollDeceleration_Snolfed()
{
    RSDK_THIS(Player);

    int32 initialVel = self->groundVel;
    if (self->right && self->groundVel < 0)
        self->groundVel += self->rollingDeceleration;

    if (self->left && self->groundVel > 0)
        self->groundVel -= self->rollingDeceleration;

    if (self->groundVel)
    {
        if (self->groundVel < 0)
        {
            self->groundVel += self->rollingFriction;

            if (RSDK.Sin256(self->angle) >= 0)
                self->groundVel += 0x1400 * RSDK.Sin256(self->angle) >> 8;
            else
                self->groundVel += 0x5000 * RSDK.Sin256(self->angle) >> 8;

            if (self->groundVel < -0x120000)
                self->groundVel = -0x120000;
        }
        else
        {
            self->groundVel -= self->rollingFriction;

            if (RSDK.Sin256(self->angle) <= 0)
                self->groundVel += 0x1400 * RSDK.Sin256(self->angle) >> 8;
            else
                self->groundVel += 0x5000 * RSDK.Sin256(self->angle) >> 8;

            if (self->groundVel > 0x120000)
                self->groundVel = 0x120000;
        }
    }
    else
    {
        self->groundVel += 0x5000 * RSDK.Sin256(self->angle) >> 8;
    }

    switch (self->collisionMode)
    {
    case CMODE_FLOOR:
        if (self->state == Player_State_TubeRoll)
        {
            if (abs(self->groundVel) < 0x10000)
            {
                if (self->direction & FLIP_Y)
                    self->groundVel = -0x40000;
                else
                    self->groundVel = 0x40000;
            }
        }
        else
        {
            if ((self->groundVel >= 0 && initialVel <= 0) || (self->groundVel <= 0 && initialVel >= 0))
            {
                self->groundVel = 0;
                if (self->controlLock) // Only unroll from a ball in cutscenes.
                {
                    self->state = Player_State_Ground;
                }
            }
        }
        break;

    case CMODE_LWALL:
    case CMODE_RWALL:
        if (self->angle >= 0x40 && self->angle <= 0xC0)
        {
            if (abs(self->groundVel) < 0x20000)
            {
                self->velocity.x = self->groundVel * RSDK.Cos256(self->angle) >> 8;
                self->velocity.y = self->groundVel * RSDK.Sin256(self->angle) >> 8;
                self->onGround = false;
                self->angle = 0;
                self->collisionMode = CMODE_FLOOR;
            }
        }
        break;

    case CMODE_ROOF:
        if (self->invertGravity)
        {
            if ((self->groundVel >= 0 && initialVel <= 0) || (self->groundVel <= 0 && initialVel >= 0))
            {
                if (self->controlLock) // Only unroll from a ball in cutscenes.
                {
                    self->groundVel = 0;
                    self->state = Player_State_Ground;
                }
            }
        }
        else
        {
            if (self->angle >= 0x40 && self->angle <= 0xC0)
            {
                if (abs(self->groundVel) < 0x20000)
                {
                    self->velocity.x = self->groundVel * RSDK.Cos256(self->angle) >> 8;
                    self->velocity.y = self->groundVel * RSDK.Sin256(self->angle) >> 8;
                    self->onGround = false;
                    self->angle = 0;
                    self->collisionMode = CMODE_FLOOR;
                }
            }
        }
        break;
    }
}

// This isn't a public method via RSDK it seems, so I have to reimplement it here.
void Player_HandleGroundAnimation_Snolfed()
{
    RSDK_THIS(Player);

    if (self->skidding > 0)
    {
        if (self->animator.animationID != ANI_SKID)
        {
            if (self->animator.animationID == ANI_SKID_TURN)
            {
                if (self->animator.frameID == self->animator.frameCount - 1)
                {
                    self->direction ^= FLIP_X;
                    self->skidding = 1;
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);
                }
            }
            else
            {
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_SKID, &self->animator, false, 0);
                if (abs(self->groundVel) >= 0x60000)
                {
                    if (abs(self->groundVel) >= 0xA0000)
                        self->animator.speed = 64;
                    else
                        self->animator.speed = 144;
                }
                else
                {
                    self->skidding -= 8;
                }

                //  [SNOLF TODO] - This segfaults and I'm not sure exactly why; probably an issue with not retrieving the correct Dust object before creating this entity.
                //  I think I'd need to use "Mod.FindObject()" to get the Dust object first.
                //  Either way, it's not a big issue since the player will very rarely be skidding (as they'll be a ball most of the time).

                // RSDK.PlaySfx(Player->sfxSkidding, false, 255);
                // EntityDust *dust = CREATE_ENTITY(Dust, self, self->position.x, self->position.y);
                // dust->state = Dust_State_DustTrail;
            }
        }
        else
        {
            int32 spd = self->animator.speed;
            if (self->direction)
            {
                if (self->groundVel >= 0)
                {
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_SKID_TURN, &self->animator, false, 0);
                }
            }
            else if (self->groundVel <= 0)
            {
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_SKID_TURN, &self->animator, false, 0);
            }

            self->animator.speed = spd;
        }

        --self->skidding;
    }
    else
    {
        if (self->pushing > -3 && self->pushing < 3)
        {
            if (self->groundVel || (self->angle >= 0x20 && self->angle <= 0xE0 && !self->invertGravity))
            {
                self->timer = 0;
                self->outtaHereTimer = 0;

                int32 velocity = abs(self->groundVel);
                if (velocity < self->minJogVelocity)
                {
                    if (self->animator.animationID == ANI_JOG)
                    {
                        if (self->animator.frameID == 9)
                            RSDK.SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 9);
                    }
                    else if (self->animator.animationID == ANI_AIR_WALK)
                    {
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, self->animator.frameID);
                    }
                    else
                    {
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);
                    }
                    self->animator.speed = (velocity >> 12) + 48;
                    self->minJogVelocity = 0x40000;
                }
                else if (velocity < self->minRunVelocity)
                {
                    if (self->animator.animationID != ANI_WALK || self->animator.frameID == 3)
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JOG, &self->animator, false, 0);
                    self->animator.speed = (velocity >> 12) + 0x40;
                    self->minJogVelocity = 0x38000;
                    self->minRunVelocity = 0x60000;
                }
                else if (velocity < self->minDashVelocity)
                {
                    if (self->animator.animationID == ANI_DASH || self->animator.animationID == ANI_RUN)
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_RUN, &self->animator, false, 0);
                    else
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_RUN, &self->animator, false, 1);

                    self->animator.speed = MIN((velocity >> 12) + 0x60, 0x200);
                    self->minRunVelocity = 0x58000;
                    self->minDashVelocity = 0xC0000;
                }
                else
                {
                    if (self->animator.animationID == ANI_DASH || self->animator.animationID == ANI_RUN)
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_DASH, &self->animator, false, 0);
                    else
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_DASH, &self->animator, false, 1);
                    self->minDashVelocity = 0xB8000;
                }
            }
            else
            {
                self->minJogVelocity = 0x40000;
                self->minRunVelocity = 0x60000;
                self->minDashVelocity = 0xC0000;

                Vector2 posStore = self->position;

                self->flailing |=
                    RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_FLOOR, self->collisionPlane, self->sensorX[0], self->sensorY, 10) << 0;
                self->flailing |=
                    RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_FLOOR, self->collisionPlane, self->sensorX[1], self->sensorY, 10) << 1;
                self->flailing |=
                    RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_FLOOR, self->collisionPlane, self->sensorX[2], self->sensorY, 10) << 2;
                self->flailing |=
                    RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_FLOOR, self->collisionPlane, self->sensorX[3], self->sensorY, 10) << 3;
                self->flailing |=
                    RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_FLOOR, self->collisionPlane, self->sensorX[4], self->sensorY, 10) << 4;

                self->position = posStore;
                switch (self->flailing)
                {
                case 0b00000001:
                case 0b00000011:
                    if (self->direction == FLIP_X || (self->characterID == ID_SONIC && self->superState == SUPERSTATE_SUPER) || self->isChibi)
                    {
                        self->direction = FLIP_X;
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_BALANCE_1, &self->animator, false, 0);
                    }
                    else
                    {
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_BALANCE_2, &self->animator, false, 0);
                    }
                    break;

                case 0b00010000:
                case 0b00011000:
                    if (self->direction && (self->characterID != ID_SONIC || self->superState != SUPERSTATE_SUPER) && !self->isChibi)
                    {
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_BALANCE_2, &self->animator, false, 0);
                    }
                    else
                    {
                        self->direction = FLIP_NONE;
                        RSDK.SetSpriteAnimation(self->aniFrames, ANI_BALANCE_1, &self->animator, false, 0);
                    }
                    break;

                // bit 5 & 6 are never set, this code cannot be reached
                // in theory, bit 5 & 6 would be set using sensorLC & sensorLR equivalents from v4/S2/S3
                case 0b01101111:
                case 0b01110100:
                    RSDK.SetSpriteAnimation(self->aniFrames, ANI_BALANCE_1, &self->animator, false, 0);
                    break;

                // Not balancing
                case 0b00000000:
                default:
                    // Player_HandleIdleAnimation();
                    break;
                }
            }
        }
        else
        {
            self->pushing = CLAMP(self->pushing, -3, 3);
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_PUSH, &self->animator, false, 0);
        }
    }
}

// In Snolf, the player cannot control themselves in the air!
void Player_HandleAirFriction_Snolfed()
{
    RSDK_THIS(Player);

    if (self->velocity.y > -0x40000 && self->velocity.y < 0)
        self->velocity.x -= self->velocity.x >> 5;

    // No air controls for YOU, buddy! Hide the effect of each direction behind self->controlLock checks.
    // NOTE: We must allow the player to move in the air if they don't have the rolled animation - so that springs don't result in softlocks.
    if (self->left)
    {
        if (self->velocity.x > -self->topSpeed && (self->controlLock || self->animator.animationID != ANI_JUMP))
            self->velocity.x -= self->airAcceleration;

        self->direction = FLIP_X;
    }

    if (self->right)
    {
        if (self->velocity.x < self->topSpeed && (self->controlLock || self->animator.animationID != ANI_JUMP))
            self->velocity.x += self->airAcceleration;

        self->direction = FLIP_NONE;
    }
}

// In SNOLF, the Player has infinite lives.
bool32 Snolf_EnsureInfiniteLives(bool32 skipped)
{
    RSDK_THIS(Player);
    self->snolfEngine.currentShotState = SNOLF_SHOT_READY;

    // Force lives to be at least 4 (which becomes 3 after death code finishes). Kinda weird to do it this way, but it's cleaner than modifying the whole death function.
    if (self->lives < 4)
    {
        RSDK.PrintLog(PRINT_NORMAL, "Setting lives to 4.");
        self->lives = 4;
    }

    return false; // Do not override.
}