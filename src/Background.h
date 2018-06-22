/** @file Background.h
 * @ingroup Background
 */

#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <SDL2/SDL.h>
#include <stdint.h>

#define BACKGROUND_SCROLL_DIRECTION 0

/**
 * @ingroup Background
 */
typedef struct Background_t {
    SDL_Texture *stLayer;
    uint16_t    u16Flags;
    double      dPosX;
    double      dPosY;
    double      dVelocity;
} Background;

int8_t      BackgroundDraw(SDL_Renderer *stRenderer, Background *stBackground);
Background *BackgroundInit(SDL_Renderer *stRenderer, const char *pcFilename, int32_t s32WindowWidth);

#endif
