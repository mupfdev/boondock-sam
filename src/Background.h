/** @file Background.h
 * @ingroup Background
 */

#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <SDL2/SDL.h>
#include <stdint.h>

/**
 * @ingroup Background
 */
enum BackgroundFlags
{
    BACKGROUND_SCROLL_DIRECTION=0
};

/**
 * @ingroup Background
 */
typedef struct Background_t
{
    SDL_Texture *pstLayer;
    uint16_t     u16Flags;
    double       dPosX;
    double       dPosY;
    double       dVelocity;
} Background;

int8_t      DrawBackground(SDL_Renderer *pstRenderer, Background *pstBackground);
Background *InitBackground(SDL_Renderer *pstRenderer, const char *pcFilename, int32_t s32WindowWidth);

#endif
