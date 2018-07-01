/**
 * @file      Video.c
 * @ingroup   Video
 * @defgroup  Video
 * @brief     Video subsystem.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Video.h"

/**
 * @brief
 * @param   pacTitle
 * @param   s32Width
 * @param   s32Height
 * @param   u8Fullscreen
 * @param   dZoomLevel
 * @return
 * @ingroup Video
 */
Video *InitVideo(
    const char    *pacTitle,
    const int32_t  s32Width,
    const int32_t  s32Height,
    const uint8_t  u8Fullscreen,
    const double   dZoomLevel)
{
    uint32_t      u32Flags;
    static Video *pstVideo;

    pstVideo = malloc(sizeof(struct Video_t));

    if (NULL == pstVideo)
    {
        fprintf(stderr, "InitVideo(): error allocating memory.\n");
        return NULL;
    }

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    pstVideo->s32WindowHeight   = s32Height;
    pstVideo->s32WindowWidth    = s32Width;
    pstVideo->dZoomLevel        = dZoomLevel;
    pstVideo->dZoomLevelInitial = dZoomLevel;

    if (u8Fullscreen)
    {
        u32Flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        u32Flags = 0;
    }

    pstVideo->pstWindow = SDL_CreateWindow(
        pacTitle,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        pstVideo->s32WindowWidth,
        pstVideo->s32WindowHeight,
        u32Flags);

    if (NULL == pstVideo->pstWindow)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    if (u8Fullscreen)
    {
        SDL_GetWindowSize(
            pstVideo->pstWindow,
            &pstVideo->s32WindowWidth,
            &pstVideo->s32WindowHeight);

        if (0 > SDL_ShowCursor(SDL_DISABLE))
        {
            fprintf(stderr, "%s\n", SDL_GetError());
            free(pstVideo);
            return NULL;
        }
    }

    pstVideo->pstRenderer = SDL_CreateRenderer(
        pstVideo->pstWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    if (NULL == pstVideo->pstRenderer)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    if (0 != SDL_RenderSetLogicalSize(
            pstVideo->pstRenderer,
            pstVideo->s32WindowWidth  / dZoomLevel,
            pstVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    return pstVideo;
}

/**
 * @brief
 * @param   pstVideo
 * @param   dZoomLevel
 * @ingroup Video
 * @return
 */
int8_t SetVideoZoomLevel(Video *pstVideo, double dZoomLevel)
{
    if (dZoomLevel <= VIDEO_MIN_ZOOMLEVEL) dZoomLevel = VIDEO_MIN_ZOOMLEVEL;
    if (dZoomLevel >= VIDEO_MAX_ZOOMLEVEL) dZoomLevel = VIDEO_MAX_ZOOMLEVEL;

    if (0 != SDL_RenderSetLogicalSize(
            pstVideo->pstRenderer,
            pstVideo->s32WindowWidth  / dZoomLevel,
            pstVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    pstVideo->dZoomLevel = dZoomLevel;

    return 0;
}

/**
 * @brief
 * @param   pstVideo
 * @ingroup Video
 */
void TerminateVideo(Video *pstVideo)
{
    if ((NULL == pstVideo->pstWindow))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
    }

    SDL_DestroyRenderer(pstVideo->pstRenderer);
    SDL_DestroyWindow(pstVideo->pstWindow);
    free(pstVideo);
}

void UpdateVideo(SDL_Renderer *pstRenderer)
{
    SDL_RenderPresent(pstRenderer);
    #ifndef __EMSCRIPTEN__
    SDL_RenderClear(pstRenderer);
    #endif
}
