/** @file Background.c
 * @ingroup   Background
 * @defgroup  Background
 * @brief     A handler to manage parallax scrolling backgrounds.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL_image.h>
#include "Background.h"

static SDL_Texture *_RenderLayer(
    SDL_Renderer  *pstRenderer,
    const char    *pcFilename,
    const int32_t  s32WindowWidth)
{
    SDL_Texture *pstImage        = NULL;
    SDL_Texture *pstLayer        = NULL;
    int32_t      s32ImageWidth  = 0;
    int32_t      s32ImageHeight = 0;
    int32_t      s32LayerHeight = 0;
    int32_t      s32LayerWidth  = 0;
    uint8_t      u8WidthFactor  = 0;

    pstImage = IMG_LoadTexture(pstRenderer, pcFilename);
    if (NULL == pstImage)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return NULL;
    }

    if (0 != SDL_QueryTexture(pstImage, NULL, NULL, &s32ImageWidth, &s32ImageHeight))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(pstImage);
        return NULL;
    }

    u8WidthFactor  = ceil((double)s32WindowWidth / (double)s32ImageWidth);
    s32LayerWidth  = s32ImageWidth * u8WidthFactor;
    s32LayerHeight = s32ImageHeight;
    pstLayer        = SDL_CreateTexture(
        pstRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        s32LayerWidth,
        s32LayerHeight);

    if (NULL == pstLayer)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(pstImage);
        return 0;
    }

    if (0 != SDL_SetRenderTarget(pstRenderer, pstLayer))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(pstLayer);
        SDL_DestroyTexture(pstImage);
        return NULL;
    }

    SDL_Rect stDst;
    stDst.x = 0;
    for (uint8_t u8Index = 0; u8Index < u8WidthFactor; u8Index++)
    {
        stDst.y  = 0;
        stDst.w  = s32ImageWidth;
        stDst.h  = s32ImageHeight;
        SDL_RenderCopy(pstRenderer, pstImage, NULL, &stDst);
        stDst.x += s32ImageWidth;
    }

    if (0 != SDL_SetTextureBlendMode(pstLayer, SDL_BLENDMODE_BLEND))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(pstLayer);
        SDL_DestroyTexture(pstImage);
        return NULL;
    }

    if (0 != SDL_SetRenderTarget(pstRenderer, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(pstLayer);
        SDL_DestroyTexture(pstImage);
        return NULL;
    }

    return pstLayer;
}

int8_t DrawBackground(SDL_Renderer *pstRenderer, Background *pstBackground)
{
    int32_t s32Width = 0;
    if (0 != SDL_QueryTexture(pstBackground->pstLayer, NULL, NULL, &s32Width, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    if (pstBackground->dPosX < -s32Width)
    {
        pstBackground->dPosX = +s32Width;
    }

    if (pstBackground->dPosX > +s32Width)
    {
        pstBackground->dPosX = -s32Width;
    }

    double dPosXa = pstBackground->dPosX;
    double dPosXb;

    if (dPosXa > 0)
    {
        dPosXb = dPosXa - s32Width;
    }
    else
    {
        dPosXb = dPosXa + s32Width;
    }

    if (pstBackground->dVelocity > 0)
    {
        if ((pstBackground->u16Flags >> BACKGROUND_SCROLL_DIRECTION) & 1)
        {
            pstBackground->dPosX += pstBackground->dVelocity;
        }
        else
        {
            pstBackground->dPosX -= pstBackground->dVelocity;
        }
    }

    SDL_Rect stDst = { dPosXa, pstBackground->dPosY, s32Width, 192 };
    if (-1 == SDL_RenderCopyEx(pstRenderer, pstBackground->pstLayer, NULL, &stDst, 0, NULL, SDL_FLIP_NONE))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    stDst.x = dPosXb;
    if (-1 == SDL_RenderCopyEx(pstRenderer, pstBackground->pstLayer, NULL, &stDst, 0, NULL, SDL_FLIP_NONE))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

Background *InitBackground(SDL_Renderer *pstRenderer, const char *pcFilename, int32_t s32WindowWidth)
{
    static Background *pstBackground;
    pstBackground = malloc(sizeof(struct Background_t));

    if (NULL == pstBackground)
    {
        fprintf(stderr, "InitBackground(): error allocating memory.\n");
        return NULL;
    }

    pstBackground->u16Flags = 0;
    pstBackground->pstLayer  = _RenderLayer(pstRenderer, pcFilename, s32WindowWidth);

    if (NULL == pstBackground->pstLayer)
    {
        free(pstBackground);
        return NULL;
    }

    pstBackground->dPosX     = 0;
    pstBackground->dPosY     = 0;
    pstBackground->dVelocity = 0;

    return pstBackground;
}
