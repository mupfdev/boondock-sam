/** @file Map.c
 * @ingroup   Map
 * @defgroup  Map
 * @brief     Handles TMX (Tile Map XML) map files that can be edited
 *            using the Tiled Map Editor.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include "tmx/tmx.h"
#include "Map.h"

/**
 * @brief   Draw map.
 * @param   pstRenderer  a SDL rendering context.
 *          See @ref struct Video.
 * @param   pstMap             
 * @param   pacLayerName       
 * @param   u8RenderBackground 
 * @param   u8Index            
 * @param   dCameraPosX        
 * @param   dCameraPosY        
 * @return  
 * @ingroup Map
 */
int8_t DrawMap(
    SDL_Renderer  *pstRenderer,
    Map           *pstMap,
    const char    *pacLayerName,
    const uint8_t  u8RenderBackground,
    const uint8_t  u8Index,
    const double   dCameraPosX,
    const double   dCameraPosY)
{
    SDL_Texture *pstTileset = NULL;
    tmx_layer   *pstLayers  = pstMap->pstTmxMap->ly_head;

    // Draw layer in case it has already been rendered.
    if (pstMap->pstLayer[u8Index])
    {
        double dRenderPosX = pstMap->dWorldPosX - dCameraPosX;
        double dRenderPosY = pstMap->dWorldPosY - dCameraPosY;

        SDL_Rect stDst =
        {
            dRenderPosX,
            dRenderPosY,
            pstMap->pstTmxMap->width  * pstMap->pstTmxMap->tile_width,
            pstMap->pstTmxMap->height * pstMap->pstTmxMap->tile_height
        };
        if (-1 == SDL_RenderCopyEx(
                pstRenderer,
                pstMap->pstLayer[u8Index],
                NULL,
                &stDst,
                0,
                NULL,
                SDL_FLIP_NONE))
        {
            fprintf(stderr, "%s\n", SDL_GetError());
            return -1;
        }
        return 0;
    }
    // Else: render layer once.
    pstMap->pstLayer[u8Index] = SDL_CreateTexture(
        pstRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        pstMap->pstTmxMap->width  * pstMap->pstTmxMap->tile_width,
        pstMap->pstTmxMap->height * pstMap->pstTmxMap->tile_height);

    if (NULL == pstMap->pstLayer[u8Index])
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    pstTileset = IMG_LoadTexture(
        pstRenderer,
        pstMap->pacTilesetImageFilename);
    if (NULL == pstTileset)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    if (0 != SDL_SetRenderTarget(pstRenderer, pstMap->pstLayer[u8Index]))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    if (u8RenderBackground)
    {
        SDL_SetRenderDrawColor(
            pstRenderer,
            (pstMap->pstTmxMap->backgroundcolor >> 16) & 0xFF,
            (pstMap->pstTmxMap->backgroundcolor >>  8) & 0xFF,
            (pstMap->pstTmxMap->backgroundcolor)       & 0xFF,
            255);
    }

    while(pstLayers)
    {
        uint32_t     u32Gid;
        SDL_Rect     stDst;
        SDL_Rect     stSrc;
        tmx_tileset *pstTS;

        if ((pstLayers->visible) && (NULL != strstr(pstLayers->name, pacLayerName)))
        {
            for (uint32_t u32IndexH = 0; u32IndexH < pstMap->pstTmxMap->height; u32IndexH++)
            {
                for (uint32_t u32IndexW = 0; u32IndexW < pstMap->pstTmxMap->width; u32IndexW++)
                {
                    u32Gid = pstLayers->content.gids[
                        (u32IndexH * pstMap->pstTmxMap->width) + u32IndexW]
                        & TMX_FLIP_BITS_REMOVAL;
                    if (NULL != pstMap->pstTmxMap->tiles[u32Gid])
                    {
                        pstTS    = pstMap->pstTmxMap->tiles[u32Gid]->tileset;
                        stSrc.x  = pstMap->pstTmxMap->tiles[u32Gid]->ul_x;
                        stSrc.y  = pstMap->pstTmxMap->tiles[u32Gid]->ul_y;
                        stSrc.w  = stDst.w   = pstTS->tile_width;
                        stSrc.h  = stDst.h   = pstTS->tile_height;
                        stDst.x  = u32IndexW * pstTS->tile_width;
                        stDst.y  = u32IndexH * pstTS->tile_height;
                        SDL_RenderCopy(pstRenderer, pstTileset, &stSrc, &stDst);
                    }
                }
            }
        }
        pstLayers = pstLayers->next;
    }

    // Switch back to default render target.
    if (0 != SDL_SetRenderTarget(pstRenderer, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    if (0 != SDL_SetTextureBlendMode(pstMap->pstLayer[u8Index], SDL_BLENDMODE_BLEND))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief   
 * @param   pstMap 
 * @ingroup Map
 */
void FreeMap(Map *pstMap)
{
    tmx_map_free(pstMap->pstTmxMap);
    free(pstMap->pacTilesetImageFilename);
    free(pstMap);
}

/**
 * @brief   
 * @param   pacFilename             
 * @param   pacTilesetImageFilename 
 * @return  
 * @ingroup Map
 */
Map  *InitMap(
    const char *pacFilename,
    const char *pacTilesetImageFilename)
{
    static Map *pstMap;
    pstMap = malloc(sizeof(struct Map_t));
    if (NULL == pstMap)
    {
        fprintf(stderr, "InitMap(): error allocating memory.\n");
        return NULL;
    }

    pstMap->pstTmxMap = tmx_load(pacFilename);
    if (NULL == pstMap->pstTmxMap)
    {
        free(pstMap);
        fprintf(stderr, "%s\n", tmx_strerr());
        return NULL;
    }

    pstMap->pacTilesetImageFilename =
        malloc(strlen(pacTilesetImageFilename) + 1);
    if (NULL == pstMap->pacTilesetImageFilename)
    {
        free(pstMap);
        fprintf(stderr, "InitMap(): error allocating memory.\n");
        return NULL;
    }
    memcpy(
        pstMap->pacTilesetImageFilename,
        pacTilesetImageFilename,
        strlen(pacTilesetImageFilename) + 1);

    pstMap->u32Height  = pstMap->pstTmxMap->height * pstMap->pstTmxMap->tile_height;
    pstMap->u32Width   = pstMap->pstTmxMap->width  * pstMap->pstTmxMap->tile_width;
    pstMap->dWorldPosX = 0;
    pstMap->dWorldPosY = 0;

    for (uint8_t u8Index = 0; u8Index < MAP_MAX_LAYERS; u8Index++)
    {
        pstMap->pstLayer[u8Index] = NULL;
    }

    return pstMap;
}

/**
 * @brief   
 * @param   pstMap  
 * @param   pacType 
 * @param   dPosX   
 * @param   dPosY   
 * @return  
 * @ingroup Map
 */
uint8_t IsMapCoordOfType(
    const Map  *pstMap,
    const char *pacType,
    double      dPosX,
    double      dPosY)
{
    dPosX /= pstMap->pstTmxMap->tile_width + 1;
    dPosY /= pstMap->pstTmxMap->tile_height;

    // Prevent segfaults by setting boundaries.
    if ( (dPosX < 0) ||
         (dPosY < 0) ||
         (dPosX > pstMap->pstTmxMap->width) ||
         (dPosY > pstMap->pstTmxMap->height) )
    {
        return 0;
    }

    tmx_layer *pstLayers = pstMap->pstTmxMap->ly_head;
    while(pstLayers)
    {
        uint16_t u16Gid =
            pstLayers->content.gids[
                ((int32_t)dPosY * pstMap->pstTmxMap->width) + (int32_t)dPosX
                ] & TMX_FLIP_BITS_REMOVAL;

        if (NULL != pstMap->pstTmxMap->tiles[u16Gid])
        {
            if (NULL != pstMap->pstTmxMap->tiles[u16Gid]->type)
            {
                if (0 == strcmp(pacType, pstMap->pstTmxMap->tiles[u16Gid]->type))
                {
                    return 1;
                }
            }
        }
        pstLayers = pstLayers->next;
    }

    return 0;
}
