/** @file Map.h
 * @ingroup Map
 */

#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include <stdint.h>
#include "tmx/tmx.h"

/**
 * @ingroup Map
 */
enum MapLimits
{
    MAP_MAX_LAYERS = 5
};

/**
 * @ingroup Map
 */
typedef struct Map_t
{
    tmx_map     *pstTmxMap;
    char        *pacTilesetImageFilename;
    SDL_Texture *pstLayer[MAP_MAX_LAYERS];
    uint32_t     u32Height;
    uint32_t     u32Width;
    double       dWorldPosX;
    double       dWorldPosY;
} Map;

int8_t DrawMap(
    SDL_Renderer  *pstRenderer,
    Map           *pstMap,
    const char    *pacLayerName,
    const uint8_t  u8RenderBackground,
    const uint8_t  u8Index,
    const double   dCameraPosX,
    const double   dCameraPosY);

void FreeMap(Map *pstMap);

Map *InitMap(
    const char *pacFilename,
    const char *pacTilesetImageFilename);

uint8_t IsMapCoordOfType(
    const Map  *pstMap,
    const char *pacType,
    double      dPosX,
    double      dPosY);

#endif
