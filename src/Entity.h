/** @file Entity.h
 * @ingroup Entity
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <SDL2/SDL.h>
#include <stdint.h>
#include "AABB.h"

/**
 * @ingroup Entity
 */
enum EntityFlags
{
    ENTITY_DIRECTION     = 0,
    ENTITY_IS_DEAD       = 1,
    ENTITY_IS_IDLING     = 2,
    ENTITY_IS_IN_MID_AIR = 3,
    ENTITY_IS_JUMPING    = 4,
    ENTITY_IS_TRAVELING  = 5,
};

/**
 * @ingroup Entity
 */
typedef struct Entity_t
{
    double       dAcceleration;
    double       dDeceleration;
    double       dJumpForce;
    uint16_t     u16Flags;
    uint8_t      u8Height;
    uint8_t      u8Width;
    uint32_t     u32MapWidth;
    uint32_t     u32MapHeight;
    double       dFrameAnimationFPS;
    uint8_t      u8FrameStart;
    uint8_t      u8FrameEnd;
    uint8_t      u8FrameOffsetY;
    double       dMaxVelocityX;
    double       dWorldMeterInPixel;
    double       dWorldGravitation;
    double       dWorldPosX;
    double       dWorldPosY;
    /* Remark: the following variables are used internally to store
     * volatile values and usually do not have to be changed
     * manually. */
    SDL_Texture *pstSprite;
    uint8_t      u8Frame;
    double       dFrameDuration;
    AABB         stBB;
    double       dInitialJumpVelocity;
    double       dInitialWorldPosX;
    double       dInitialWorldPosY;
    double       dInitialWorldGravitation;
    double       dVelocityX;
    double       dVelocityY;
    double       dDistanceX;
    double       dDistanceY;
} Entity;

int8_t DrawEntity(
    SDL_Renderer *pstRenderer,
    Entity       *pstEntity,
    double        dCameraPosX,
    double        dCameraPosY);

void FixEntityPositionY(Entity *pstEntity);

Entity *InitEntity(
    const uint8_t  u8Width,
    const uint8_t  u8Height,
    const double   dPosX,
    const double   dPosY,
    const uint32_t u32MapWidth,
    const uint32_t u32MapHeight
);

int8_t IsEntityJumping(Entity *pstEntity);

int8_t LoadEntitySprite(
    Entity       *pstEntity,
    SDL_Renderer *pstRenderer,
    const char   *pacFilename);

void ResurrectEntity(Entity *pstEntity);

void SetEntitySpriteAnimation(
    Entity  *pstEntity,
    uint8_t  u8FrameStart,
    uint8_t  u8FrameEnd,
    uint8_t  u8FrameOffsetY,
    double   dFrameAnimationFPS
);

void UpdateEntity(
    Entity *pstEntity,
    double  dDeltaTime);

#endif // _ENTITY_H_
