/** @file Main.c
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include "Background.h"
#include "Video.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define EXIT_UNSET 2

static int32_t _s32ExecStatus = EXIT_UNSET;

typedef struct Bundle_t
{
    Video  *Window;
    double  dTimeA;
    double  dTimeB;
    double  dDeltaTime;
} Bundle;

static void _MainLoop(void *arg)
{
    Bundle *Carriage     = (Bundle *)arg;
    Carriage->dTimeB     = SDL_GetTicks();
    Carriage->dDeltaTime = (Carriage->dTimeB - Carriage->dTimeA) / 1000;
    Carriage->dTimeA     = Carriage->dTimeB;

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

    #ifdef __EMSCRIPTEN__
    if (EXIT_UNSET != s32ExecStatus)
    {
        emscripten_cancel_main_loop();
    }
    #endif
}

int32_t main()
{
    Video  *stVideo            = NULL;
    Bundle *stMainLoopCarriage = NULL;

    stVideo = InitVideo("Boondock Sam", 800, 600, 1, 2);
    if (NULL == stVideo)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }
    atexit(SDL_Quit);

    double dZoomLevel = 1 + stVideo->s32WindowHeight / 216; // Background height.
    SetVideoZoomLevel(stVideo, dZoomLevel);

    stMainLoopCarriage = malloc(sizeof(struct Bundle_t));
    if (NULL == stMainLoopCarriage)
    {
        fprintf(stderr, "stMainLoopCarriage: error allocating memory.\n");
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(_MainLoop, (void *)stMainLoopCarriage, 60, 1);
    #else
    while(1)
    {
        _MainLoop((void *)stMainLoopCarriage);
        if (EXIT_UNSET != _s32ExecStatus) goto quit;
        // Limit frames per second.
        SDL_Delay((1000 / 60) - stMainLoopCarriage->dDeltaTime);
    }
    #endif

quit:
    free(stMainLoopCarriage);
    TerminateVideo(stVideo);
    return _s32ExecStatus;
}
