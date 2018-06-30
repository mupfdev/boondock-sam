/** @file Entity.c
 * @ingroup   Entity
 * @defgroup  Entity
 * @brief     Entity handler to manage all entities such as players,
 *            NPCs, etc.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include "AABB.h"
#include "Entity.h"

/**
 * @brief   Initialise entity.
 * @param   u8Height height in pixel.
 * @param   u8Width  width in pixel.
 * @param   dPosX    the initial position along the x-axis.
 * @param   dPosY    the initial position along the y-axis.
 * @return  Entity on success, NULL on error.  See @ref struct Entity.
 * @ingroup Entity
 */
Entity *InitEntity(
    const uint8_t u8Height,
    const uint8_t u8Width,
    const double  dPosX,
    const double  dPosY)
{
    static Entity *pstEntity;
    pstEntity = malloc(sizeof(struct Entity_t));
    if (NULL == pstEntity)
    {
        fprintf(stderr, "InitEntity(): error allocating memory.\n");
        return NULL;
    }

    pstEntity->dAcceleration     = 400;
    pstEntity->u16Flags          =   0;
    pstEntity->u8Height          = u8Height;
    pstEntity->u8Width           = u8Width;
    pstEntity->dVelocityMax      = 100;
    pstEntity->dWorldGravitation =   9.81;
    pstEntity->dWorldPosX        = dPosX;
    pstEntity->dWorldPosY        = dPosY;

    pstEntity->stBB.dBottom      =   0;
    pstEntity->stBB.dLeft        = u8Height;
    pstEntity->stBB.dRight       = u8Width;
    pstEntity->stBB.dTop         =   0;
    pstEntity->dInitialWorldPosX = dPosX;
    pstEntity->dInitialWorldPosY = dPosY;
    pstEntity->pstSprite         = NULL;
    pstEntity->dVelocityX        =   0;
    pstEntity->dVelocityY        =   0;

    return pstEntity;
}

/**
 * @brief   Respawn entity.
 * @param   pstEntity the entity to respawn.  See @ref struct Entity.
 * @ingroup Entity
 */
void RespawnEntity(Entity *pstEntity)
{
    pstEntity->u16Flags   &= ~(1 << ENTITY_IS_DEAD);
    pstEntity->u16Flags   &= ~(1 << ENTITY_IS_IN_MOTION);
    pstEntity->dWorldPosX  = pstEntity->dInitialWorldPosX;
    pstEntity->dWorldPosY  = pstEntity->dInitialWorldPosY;
}
