/**
 * @file      Entity.c
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
 * @brief   Draw Entity on screen.
 * @param   pstRenderer a SDL rendering context.  See @ref struct Video.
 * @param   pstEntity   an Entity.  See @ref struct Entity.
 * @param   dCameraPosX the camera position along the x-axis.
 * @param   dCameraPosY the camera position along the y-axis.
 * @return  0 on success, -1 on failure.
 * @ingroup Entity
 */
int8_t DrawEntity(
    SDL_Renderer *pstRenderer,
    Entity       *pstEntity,
    double        dCameraPosX,
    double        dCameraPosY)
{
    double           dRenderPosX;
    double           dRenderPosY;
    SDL_Rect         stDst;
    SDL_Rect         stSrc;
    SDL_RendererFlip s8Flip;

    if (NULL == pstEntity->pstSprite)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    dRenderPosX  = pstEntity->dWorldPosX - dCameraPosX;
    dRenderPosY = pstEntity->dWorldPosY - dCameraPosY;
    stDst.x      = dRenderPosX;
    stDst.y      = dRenderPosY;
    stDst.w      = pstEntity->u8Width;
    stDst.h      = pstEntity->u8Height;
    stSrc.x      = pstEntity->u8Frame * pstEntity->u8Width;
    stSrc.y      = 0;
    stSrc.w      = pstEntity->u8Width;
    stSrc.h      = pstEntity->u8Width;

    if ((pstEntity->u16Flags >> ENTITY_DIRECTION) & 1)
    {
        s8Flip = SDL_FLIP_HORIZONTAL;
    }
    else
    {
        s8Flip = SDL_FLIP_NONE;
    }

    if (-1 == SDL_RenderCopyEx(
            pstRenderer,
            pstEntity->pstSprite,
            &stSrc,
            &stDst,
            0,
            NULL,
            s8Flip))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief   Initialise Entity.
 * @param   u8Height height of the Entity in pixel.
 * @param   u8Width  width  of the Entity in pixel.
 * @param   dPosX    initial world position along the x-axis.
 * @param   dPosY    initial world position along the y-axis.
 * @return  an Entity on success, NULL on failure.
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

    pstEntity->u8Frame           =   0;
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
 * @brief   Load the Entity's sprite image.
 * @param   pstEntity   an Entity.  See @ref struct Entity.
 * @param   pstRenderer a SDL rendering context.  See @ref struct Video.
 * @param   pacFilename the filename of the image.
 * @return  0 on success, -1 on failure.
 * @ingroup Entity
 */
int8_t LoadEntitySprite(
    Entity       *pstEntity,
    SDL_Renderer *pstRenderer,
    const char   *pacFilename)
{
    if (NULL != pstEntity->pstSprite)
    {
        SDL_DestroyTexture(pstEntity->pstSprite);
    }

    pstEntity->pstSprite = IMG_LoadTexture(pstRenderer, pacFilename);
    if (NULL == pstEntity->pstSprite)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief   Resurrect Entity.
 * @param   pstEntity an Entity.  See @ref struct Entity.
 * @ingroup Entity
 */
void ResurrectEntity(Entity *pstEntity)
{
    pstEntity->u16Flags   &= ~(1 << ENTITY_IS_DEAD);
    pstEntity->u16Flags   &= ~(1 << ENTITY_IS_IN_MOTION);
    pstEntity->dWorldPosX  = pstEntity->dInitialWorldPosX;
    pstEntity->dWorldPosY  = pstEntity->dInitialWorldPosY;
}

/*
 * @brief   
 * @param   pstEentity 
 * @param   dDeltaTime 
 * @ingroup Entity
 *
void UpdateEntity(
    Entity *pstEntity,
    double dDeltaTime)
{

}*/
