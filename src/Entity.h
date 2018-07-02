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
    ENTITY_IS_IN_MID_AIR = 1,
    ENTITY_IS_IN_MOTION  = 2,
    ENTITY_IS_DEAD       = 3,
    ENTITY_IS_JUMPING    = 4
};

/**
 * @ingroup Entity
 */
typedef struct Entity_t
{
    double       dAcceleration;
    uint16_t     u16Flags;
    uint8_t      u8Height;
    uint8_t      u8Width;
    double       dVelocityMax;
    double       dWorldGravitation;
    double       dWorldPosX;
    double       dWorldPosY;
    /* Remark: the following variables are used internally to store
     * volatile values and usually do not have to be changed
     * manually. */
    AABB         stBB;
    uint8_t      u8Frame;
    double       dInitialWorldPosX;
    double       dInitialWorldPosY;
    SDL_Texture *pstSprite;
    double       dVelocityX;
    double       dVelocityY;
} Entity;

int8_t DrawEntity(
    SDL_Renderer *pstRenderer,
    Entity       *pstEntity,
    double        dCameraPosX,
    double        dCameraPosY);

Entity *InitEntity(
    const uint8_t u8Height,
    const uint8_t u8Width,
    const double  dPosX,
    const double  dPosY);

int8_t LoadEntitySprite(
    Entity       *pstEntity,
    SDL_Renderer *pstrenderer,
    const char   *pacFilename);

void ResurrectEntity(Entity *pstEntity);

void UpdateEntity(
    Entity *pstEntity,
    double dDeltaTime);

#endif
