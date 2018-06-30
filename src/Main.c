/** @file Main.c
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include "AABB.h"
#include "Background.h"
#include "Config.h"
#include "Map.h"
#include "Video.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define EXIT_UNSET 2
static  int32_t _s32ExecStatus = EXIT_UNSET;

/* This structure is used to avoid redundant global variables.  It works
 * as a carrier between the main() and the _MainLoop() function which is
 * required by Emscripten.
 */
typedef struct MainLoopBundle_t
{
    Video      *pstVideo;
    Map        *pstMap;
    Background *pstBG[5];
    double      dTimeA;
    double      dTimeB;
    double      dDeltaTime;
} MainLoopBundle;

static void _MainLoop(void *arg)
{
    MainLoopBundle *pstBundle = (MainLoopBundle *)arg;
    double dZoomLevel         = pstBundle->pstVideo->dZoomLevel;
    pstBundle->dTimeB         = SDL_GetTicks();
    pstBundle->dDeltaTime     = (pstBundle->dTimeB - pstBundle->dTimeA) / 1000;
    pstBundle->dTimeA         = pstBundle->dTimeB;

    const uint8_t *u8KeyState;
    SDL_PumpEvents();
    if (SDL_PeepEvents(0, 0, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0)
    {
        _s32ExecStatus = EXIT_FAILURE;
    }
    u8KeyState = SDL_GetKeyboardState(NULL);

    if (u8KeyState[SDL_SCANCODE_Q])
    {
        _s32ExecStatus = EXIT_SUCCESS;
    }

    if (u8KeyState[SDL_SCANCODE_0])
    {
        SetVideoZoomLevel(
            pstBundle->pstVideo,
            pstBundle->pstVideo->dZoomLevelInitial);
    }

    if (u8KeyState[SDL_SCANCODE_1])
    {
        dZoomLevel -= pstBundle->dDeltaTime;
        SetVideoZoomLevel(pstBundle->pstVideo, dZoomLevel);
    }

    if (u8KeyState[SDL_SCANCODE_2])
    {
        dZoomLevel += pstBundle->dDeltaTime;
        SetVideoZoomLevel(pstBundle->pstVideo, dZoomLevel);
    }

    pstBundle->pstBG[4]->dVelocity = pstBundle->dDeltaTime * 50;
    pstBundle->pstBG[3]->dVelocity = pstBundle->pstBG[4]->dVelocity / 2;
    pstBundle->pstBG[2]->dVelocity = pstBundle->pstBG[4]->dVelocity / 3;
    pstBundle->pstBG[1]->dVelocity = pstBundle->pstBG[4]->dVelocity / 4;
    pstBundle->pstBG[0]->dVelocity = pstBundle->pstBG[4]->dVelocity / 5;

    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        DrawBackground(
            pstBundle->pstVideo->pstRenderer,
            pstBundle->pstBG[u8Index]);
    }

    DrawMap(
        pstBundle->pstVideo->pstRenderer,
        pstBundle->pstMap,
        "Level",
        1, 0, 0, 0);
    UpdateVideo(pstBundle->pstVideo->pstRenderer);

    #ifdef __EMSCRIPTEN__
    if (EXIT_UNSET != _s32ExecStatus)
    {
        emscripten_cancel_main_loop();
    }
    #endif
}

int32_t main(int32_t s32ArgC, char *pacArgV[])
{
    const char *pacConfigFilename;
    if (s32ArgC > 1)
    {
        pacConfigFilename = pacArgV[1];
    }
    else
    {
        pacConfigFilename = "default.ini";
    }

    Background     *pstBG[5];
    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        pstBG[u8Index] = NULL;
    }

    Config          stConfig   = InitConfig(pacConfigFilename);
    MainLoopBundle *pstBundle  = NULL;
    Map            *pstMap     = NULL;
    Video          *pstVideo   = NULL;

    pstVideo = InitVideo(
        "Boondock Sam",
        stConfig.stVideo.s32Width,
        stConfig.stVideo.s32Height,
        stConfig.stVideo.s8Fullscreen,
        2);
    if (NULL == pstVideo)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }
    atexit(SDL_Quit);

    const char *pacBackgroundList[5] = {
        "res/backgrounds/plx-1.png",
        "res/backgrounds/plx-2.png",
        "res/backgrounds/plx-3.png",
        "res/backgrounds/plx-4.png",
        "res/backgrounds/plx-5.png"
    };

    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        pstBG[u8Index] = InitBackground(
            pstVideo->pstRenderer,
            pacBackgroundList[u8Index],
            pstVideo->s32WindowWidth);

        if (NULL == pstBG[u8Index])
        {
            _s32ExecStatus = EXIT_FAILURE;
            goto quit;
        }
    }

    pstMap = InitMap("res/maps/demo.tmx", "res/tilesets/jungle.png");
    if (NULL == pstMap)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }

    pstBundle = malloc(sizeof(struct MainLoopBundle_t));
    if (NULL == pstBundle)
    {
        fprintf(stderr, "stBundle: error allocating memory.\n");
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }

    pstBundle->pstVideo = pstVideo;
    pstBundle->pstMap   = pstMap;
    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        pstBundle->pstBG[u8Index] = pstBG[u8Index];
    }
    pstBundle->dTimeA   = SDL_GetTicks();

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(_MainLoop, (void *)pstBundle, 0, 1);
    #else
    while(1)
    {
        if (EXIT_UNSET != _s32ExecStatus) goto quit;
        _MainLoop((void *)pstBundle);

        if (stConfig.stVideo.s8LimitFPS)
        {
            SDL_Delay((1000 / stConfig.stVideo.s8FPS) - pstBundle->dDeltaTime);
        }
    }
    #endif

quit:
    free(pstBundle);
    FreeMap(pstMap);
    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        free(pstBG[u8Index]);
    }
    TerminateVideo(pstVideo);
    return _s32ExecStatus;
}
