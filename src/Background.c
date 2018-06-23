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
    SDL_Renderer  *stRenderer,
    const char    *pcFilename,
    const int32_t  s32WindowWidth)
{
    SDL_Texture *stImage        = NULL;
    SDL_Texture *stLayer        = NULL;
    int32_t      s32ImageWidth  = 0;
    int32_t      s32ImageHeight = 0;
    int32_t      s32LayerHeight = 0;
    int32_t      s32LayerWidth  = 0;
    uint8_t      u8WidthFactor  = 0;

    stImage = IMG_LoadTexture(stRenderer, pcFilename);
    if (NULL == stImage)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return NULL;
    }

    if (0 != SDL_QueryTexture(stImage, NULL, NULL, &s32ImageWidth, &s32ImageHeight))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(stImage);
        return NULL;
    }

    u8WidthFactor  = ceil((double)s32WindowWidth / (double)s32ImageWidth);
    s32LayerWidth  = s32ImageWidth * u8WidthFactor;
    s32LayerHeight = s32ImageHeight;
    stLayer        = SDL_CreateTexture(
        stRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        s32LayerWidth,
        s32LayerHeight);

    if (NULL == stLayer)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(stImage);
        return 0;
    }

    if (0 != SDL_SetRenderTarget(stRenderer, stLayer))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(stLayer);
        SDL_DestroyTexture(stImage);
        return NULL;
    }

    SDL_Rect stDst;
    stDst.x = 0;
    for (uint8_t u8Index = 0; u8Index < u8WidthFactor; u8Index++)
    {
        stDst.y  = 0;
        stDst.w  = s32ImageWidth;
        stDst.h  = s32ImageHeight;
        SDL_RenderCopy(stRenderer, stImage, NULL, &stDst);
        stDst.x += s32ImageWidth;
    }

    if (0 != SDL_SetTextureBlendMode(stLayer, SDL_BLENDMODE_BLEND))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(stLayer);
        SDL_DestroyTexture(stImage);
        return NULL;
    }

    if (0 != SDL_SetRenderTarget(stRenderer, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroyTexture(stLayer);
        SDL_DestroyTexture(stImage);
        return NULL;
    }

    return stLayer;
}

int8_t DrawBackground(SDL_Renderer *stRenderer, Background *stBackground)
{
    int32_t s32Width = 0;
    if (0 != SDL_QueryTexture(stBackground->stLayer, NULL, NULL, &s32Width, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    if (stBackground->dPosX < -s32Width)
    {
        stBackground->dPosX = +s32Width;
    }

    if (stBackground->dPosX > +s32Width)
    {
        stBackground->dPosX = -s32Width;
    }

    double dPosXa = stBackground->dPosX;
    double dPosXb;

    if (dPosXa > 0)
    {
        dPosXb = dPosXa - s32Width;
    }
    else
    {
        dPosXb = dPosXa + s32Width;
    }

    if (stBackground->dVelocity > 0)
    {
        if ((stBackground->u16Flags >> BACKGROUND_SCROLL_DIRECTION) & 1)
        {
            stBackground->dPosX += stBackground->dVelocity;
        }
        else
        {
            stBackground->dPosX -= stBackground->dVelocity;
        }
    }

    SDL_Rect stDst = { dPosXa, stBackground->dPosY, s32Width, 192 };
    if (-1 == SDL_RenderCopyEx(stRenderer, stBackground->stLayer, NULL, &stDst, 0, NULL, SDL_FLIP_NONE))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    stDst.x = dPosXb;
    if (-1 == SDL_RenderCopyEx(stRenderer, stBackground->stLayer, NULL, &stDst, 0, NULL, SDL_FLIP_NONE))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

Background *InitBackground(SDL_Renderer *stRenderer, const char *pcFilename, int32_t s32WindowWidth)
{
    static Background *stBackground;
    stBackground = malloc(sizeof(struct Background_t));

    if (NULL == stBackground)
    {
        fprintf(stderr, "InitBackground(): error allocating memory.\n");
        return NULL;
    }

    stBackground->u16Flags = 0;
    stBackground->stLayer  = _RenderLayer(stRenderer, pcFilename, s32WindowWidth);

    if (NULL == stBackground->stLayer)
    {
        free(stBackground);
        return NULL;
    }

    stBackground->dPosX     = 0;
    stBackground->dPosY     = 0;
    stBackground->dVelocity = 0;

    return stBackground;
}
