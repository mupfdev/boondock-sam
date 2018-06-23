/** @file Video.c
 * @ingroup   Video
 * @defgroup  Video
 * @brief     Video subsystem.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdio.h>
#include <stdlib.h>
#include "Video.h"

/**
 * @brief   Initialise video subsystem.
 * @param   pcTitle      the title of the window, in UTF-8 encoding.
 * @param   s32Width     the width of the window, in screen coordinates.
 * @param   s32Height    the height of the window, in screen coordinates.
 * @param   u8Fullscreen the window's fullscreen state.
 * @param   dZoomLevel   the zoom level used by the renderer.
 * @return  A Video structure or NULL on failure.
 *          See @ref struct Video.
 * @ingroup Video
 */
Video *InitVideo(
    const char    *pcTitle,
    const int32_t  s32Width,
    const int32_t  s32Height,
    const uint8_t  u8Fullscreen,
    const double   dZoomLevel)
{
    static Video *stVideo;
    stVideo = malloc(sizeof(struct Video_t));

    if (NULL == stVideo)
    {
        fprintf(stderr, "InitVideo(): error allocating memory.\n");
        return NULL;
    }

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(stVideo);
        return NULL;
    }

    stVideo->s32WindowHeight  = s32Height;
    stVideo->s32WindowWidth   = s32Width;
    stVideo->dZoomLevel       = dZoomLevel;
    stVideo->dZoomLevelInital = dZoomLevel;

    uint32_t u32Flags;
    if (u8Fullscreen)
    {
        u32Flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        u32Flags = 0;
    }

    stVideo->stWindow = SDL_CreateWindow(
        pcTitle,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        stVideo->s32WindowWidth,
        stVideo->s32WindowHeight,
        u32Flags);

    if (NULL == stVideo->stWindow)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(stVideo);
        return NULL;
    }

    if (u8Fullscreen)
    {
        SDL_GetWindowSize(stVideo->stWindow, &stVideo->s32WindowWidth, &stVideo->s32WindowHeight);
        if (0 > SDL_ShowCursor(SDL_DISABLE))
        {
            fprintf(stderr, "%s\n", SDL_GetError());
            free(stVideo);
            return NULL;
        }
    }

    stVideo->stRenderer = SDL_CreateRenderer(
        stVideo->stWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    if (NULL == stVideo->stRenderer)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(stVideo);
        return NULL;
    }

    if (0 != SDL_RenderSetLogicalSize(
            stVideo->stRenderer,
            stVideo->s32WindowWidth  / dZoomLevel,
            stVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(stVideo);
        return NULL;
    }

    return stVideo;
}

/**
 * @brief   Set the renderer's zoom level.
 * @param   stVideo    A Video structure.  See @ref struct Video.
 * @param   dZoomLevel the zoom level
 * @ingroup Video
 * @return  0 on success, -1 on failure.
 */
int8_t SetVideoZoomLevel(Video *stVideo, double dZoomLevel)
{
    if (dZoomLevel < 1) dZoomLevel = 1;

    if (0 != SDL_RenderSetLogicalSize(
            stVideo->stRenderer,
            stVideo->s32WindowWidth  / dZoomLevel,
            stVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    stVideo->dZoomLevel = dZoomLevel;

    return 0;
}

/**
 * @brief   Terminate video subsystem.
 * @param   stVideo a Video structure.  See @ref struct Video.
 * @ingroup Video
 */
void TerminateVideo(Video *stVideo)
{
    if ((NULL == stVideo->stWindow))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
    }

    SDL_DestroyRenderer(stVideo->stRenderer);
    SDL_DestroyWindow(stVideo->stWindow);
    free(stVideo);
}

void UpdateVideo(SDL_Renderer *stRenderer)
{
    SDL_RenderPresent(stRenderer);
    SDL_RenderClear(stRenderer);
}
