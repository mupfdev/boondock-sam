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
#include "Macros.h"

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

    dRenderPosX = pstEntity->dWorldPosX - dCameraPosX;
    dRenderPosY = pstEntity->dWorldPosY - dCameraPosY;
    stDst.x     = dRenderPosX;
    stDst.y     = dRenderPosY;
    stDst.w     = pstEntity->u8Width;
    stDst.h     = pstEntity->u8Height;
    stSrc.x     = pstEntity->u8Frame        * pstEntity->u8Width;
    stSrc.y     = pstEntity->u8FrameOffsetY * pstEntity->u8Height;
    stSrc.w     = pstEntity->u8Width;
    stSrc.h     = pstEntity->u8Height;

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
 * @brief   Fix entity position along the y-axis.  Experimental.
 * @param   pstEntity an Entity.  See @ref struct Entity.
 * @ingroup Entity
 */
void FixEntityPositionY(Entity *pstEntity)
{
    while (0 != ((int32_t)pstEntity->dWorldPosY % 8))
    {
        pstEntity->dWorldPosY = floor(pstEntity->dWorldPosY);
        pstEntity->dWorldPosY -= 1.0;
    }
}

/**
 * @brief   Initialise Entity.
 * @param   u8Width      width  of the Entity in pixel.
 * @param   u8Height     height of the Entity in pixel.
 * @param   dPosX        initial world position along the x-axis.
 * @param   dPosY        initial world position along the y-axis.
 * @param   u32MapWidth  width  of the map.  See @ref struct Map.
 * @param   u32MapHeight height of the map.  See @ref struct Map.
 * @return  an Entity on success, NULL on failure.
 * @ingroup Entity
 */
Entity *InitEntity(
    const uint8_t  u8Width,
    const uint8_t  u8Height,
    const double   dPosX,
    const double   dPosY,
    const uint32_t u32MapWidth,
    const uint32_t u32MapHeight)
{
    static Entity *pstEntity;
    pstEntity = malloc(sizeof(struct Entity_t));
    if (NULL == pstEntity)
    {
        fprintf(stderr, "InitEntity(): error allocating memory.\n");
        return NULL;
    }

    pstEntity->dAcceleration        =   5.0;
    pstEntity->dDeceleration        =   5.0;
    pstEntity->dJumpForce           =   4.0;
    pstEntity->u16Flags             =   0;
    pstEntity->u8Height             = u8Height;
    pstEntity->u8Width              = u8Width;
    pstEntity->u32MapWidth          = u32MapWidth;
    pstEntity->u32MapHeight         = u32MapHeight;
    pstEntity->dFrameAnimationFPS   =  20;
    pstEntity->u8FrameStart         =   0;
    pstEntity->u8FrameEnd           =  12;
    pstEntity->u8FrameOffsetY       =   0;
    pstEntity->dMaxVelocityX        =   3.0;
    pstEntity->dWorldMeterInPixel   =  48.0;
    pstEntity->dWorldGravitation    =   9.81;
    pstEntity->dWorldPosX           = dPosX;
    pstEntity->dWorldPosY           = dPosY;

    pstEntity->pstSprite            = NULL;
    pstEntity->u8Frame              =   0;
    pstEntity->dFrameDuration       =   0.0;
    pstEntity->stBB.dBottom         =   0;
    pstEntity->stBB.dLeft           = u8Height;
    pstEntity->stBB.dRight          = u8Width;
    pstEntity->stBB.dTop            =   0;
    pstEntity->dInitialJumpVelocity =   0.0;
    pstEntity->dInitialWorldPosX    = dPosX;
    pstEntity->dInitialWorldPosY    = dPosY;
    pstEntity->dVelocityX           =   0.0;
    pstEntity->dVelocityY           =   0.0;
    pstEntity->dDistanceX           =   0.0;
    pstEntity->dDistanceY           =   0.0;

    return pstEntity;
}

/**
 * @brief   Check if entity is jumping.
 * @param   pstEntity   an Entity.  See @ref struct Entity.
 * @return  1 if entity is jumping, 0 if not.
 * @ingroup Entity
 */
int8_t IsEntityJumping(Entity *pstEntity)
{
    if (0.0 < pstEntity->dVelocityY)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief   Load the entity's sprite image.
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
    pstEntity->u16Flags   &= ~(1 << ENTITY_IS_TRAVELING);
    pstEntity->dWorldPosX  = pstEntity->dInitialWorldPosX;
    pstEntity->dWorldPosY  = pstEntity->dInitialWorldPosY;
}

/**
 * @brief   Set the sprite animation of an Entity.
 * @param   pstEntity          an Entity.  See @ref strucht Entity.
 * @param   u8FrameStart       the animation's first frame.
 * @param   u8FrameEnd         the animation's last frame.
 * @param   u8FrameOffsetY     image offset along the y-axis.
 * @param   dFrameAnimationFPS animation speed in frames per second.
 * @ingroup Entity
 */
void SetEntitySpriteAnimation(
    Entity  *pstEntity,
    uint8_t  u8FrameStart,
    uint8_t  u8FrameEnd,
    uint8_t  u8FrameOffsetY,
    double   dFrameAnimationFPS
)
{
    pstEntity->u8FrameStart       = u8FrameStart;
    pstEntity->u8FrameEnd         = u8FrameEnd;
    pstEntity->u8FrameOffsetY     = u8FrameOffsetY;
    pstEntity->dFrameAnimationFPS = dFrameAnimationFPS;
}

/**
 * @brief   Update Entity.  This function has to be called every frame.
 * @param   pstEentity an Entity.  See @ref struct Entity.
 * @param   dDeltaTime time since last frame in seconds.
 * @ingroup Entity
 */
void UpdateEntity(
    Entity *pstEntity,
    double  dDeltaTime)
{
    // Update bounding box.
    pstEntity->stBB.dBottom = pstEntity->dWorldPosY + pstEntity->u8Height;
    pstEntity->stBB.dLeft   = pstEntity->dWorldPosX;
    pstEntity->stBB.dRight  = pstEntity->dWorldPosX + pstEntity->u8Width;
    pstEntity->stBB.dTop    = pstEntity->dWorldPosY;

    // Increase vertical velocity when entity is in mid air.
    if (FLAG_IS_NOT_SET(pstEntity->u16Flags, ENTITY_IS_IN_MID_AIR))
    {
        if (FLAG_IS_SET(pstEntity->u16Flags, ENTITY_IS_JUMPING))
        {
            pstEntity->dInitialJumpVelocity = pstEntity->dVelocityX;
            pstEntity->dVelocityY =
                -pstEntity->dJumpForce
                + -pstEntity->dInitialJumpVelocity
                * dDeltaTime;

            FLAG_SET(pstEntity->u16Flags, ENTITY_IS_IN_MID_AIR);
        }
    }

    if (FLAG_IS_SET(pstEntity->u16Flags, ENTITY_IS_IN_MID_AIR))
    {
        if (IsEntityJumping(pstEntity))
        {
            FLAG_CLEAR(pstEntity->u16Flags, ENTITY_IS_JUMPING);
        }

        double dG = pstEntity->dWorldMeterInPixel * pstEntity->dWorldGravitation;
        pstEntity->dDistanceY  = dG * dDeltaTime * dDeltaTime;
        pstEntity->dVelocityY += pstEntity->dDistanceY;
        pstEntity->dWorldPosY += pstEntity->dVelocityY;
    }
    else
    {
        FixEntityPositionY(pstEntity);
        pstEntity->dVelocityY           = 0;
        pstEntity->dInitialJumpVelocity = 0;
    }

    // Increase/decrease horizontal velocity if entity is traveling.
    if (FLAG_IS_SET(pstEntity->u16Flags, ENTITY_IS_TRAVELING))
    {
        pstEntity->dDistanceX =
            pstEntity->dWorldMeterInPixel
            * pstEntity->dAcceleration
            * dDeltaTime
            * dDeltaTime;

        pstEntity->dVelocityX += pstEntity->dDistanceX;
    }
    else
    {
        pstEntity->dVelocityX -= pstEntity->dDeceleration * dDeltaTime;
    }

    // Set horizontal position.
    if (pstEntity->dVelocityX > 0)
    {
        if (FLAG_IS_SET(pstEntity->u16Flags, ENTITY_DIRECTION))
        {
            pstEntity->dWorldPosX -= pstEntity->dVelocityX;
        }
        else
        {
            pstEntity->dWorldPosX += pstEntity->dVelocityX;
        }
    }

    // Set horizontal velocity limits.
    if (pstEntity->dVelocityX >= pstEntity->dMaxVelocityX)
    {
        pstEntity->dVelocityX = pstEntity->dMaxVelocityX;
    }
    if (pstEntity->dVelocityX < 0) { pstEntity->dVelocityX = 0; }

    // Connect left and right map border and vice versa.
    if (pstEntity->dWorldPosX <= 0)
    {
        pstEntity->dWorldPosX = pstEntity->u32MapWidth - pstEntity->u8Width;
    }
    else if (pstEntity->dWorldPosX >= pstEntity->u32MapWidth - pstEntity->u8Width)
    {
        pstEntity->dWorldPosX = 0;
    }

    // Kill entity when it falls out of the map.
    if (pstEntity->dWorldPosY >= pstEntity->u32MapHeight + pstEntity->u8Height)
    {
        FLAG_SET(pstEntity->u16Flags, ENTITY_IS_DEAD);
    }

    if (pstEntity->dWorldPosY >= pstEntity->u32MapHeight + pstEntity->u8Height)
    {
        pstEntity->dWorldPosY = pstEntity->u32MapHeight + pstEntity->u8Height;
    }

    // Update frame.
    pstEntity->dFrameDuration += dDeltaTime;

    if (pstEntity->u8Frame < pstEntity->u8FrameStart)
    {
        pstEntity->u8Frame = pstEntity->u8FrameStart;
    }

    if (pstEntity->dFrameDuration > 1 / pstEntity->dFrameAnimationFPS)
    {
        pstEntity->u8Frame++;
        pstEntity->dFrameDuration = 0;
    }

    // Loop frame animation.
    if (pstEntity->u8Frame >= pstEntity->u8FrameEnd)
    {
        pstEntity->u8Frame = pstEntity->u8FrameStart;
    }
}
