/**
 * @file      Main.c
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include "AABB.h"
#include "Background.h"
#include "Config.h"
#include "Entity.h"
#include "Macros.h"
#include "Map.h"
#include "Video.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define CAMERA_IS_LOCKED 0
#define EXIT_UNSET       2
static  int32_t _s32ExecStatus = EXIT_UNSET;

/**
 * @brief This structure is used to avoid redundant global variables.
 * It works as a carrier between the main() and the _MainLoop() function
 * which is required by Emscripten.
 */
typedef struct MainLoopBundle_t
{
    Background *pstBG[5];
    Entity     *pstSam;
    Map        *pstMap;
    Video      *pstVideo;
    double      dTimeA;
    double      dTimeB;
    double      dDeltaTime;
    double      dCameraPosX;
    double      dCameraPosY;
    double      dCameraMaxPosX;
    double      dCameraMaxPosY;
} MainLoopBundle;

static void _MainLoop(void *pArg)
{
    uint16_t        u16Flags  = 0;
    MainLoopBundle *pstBundle = (MainLoopBundle *)pArg;
    pstBundle->dTimeB         = SDL_GetTicks();
    pstBundle->dDeltaTime     = (pstBundle->dTimeB - pstBundle->dTimeA) / 1000;
    pstBundle->dTimeA         = pstBundle->dTimeB;

    // Process keyboard input.
    const uint8_t *u8KeyState;
    SDL_PumpEvents();
    if (SDL_PeepEvents(0, 0, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0)
    {
        _s32ExecStatus = EXIT_FAILURE;
    }
    u8KeyState = SDL_GetKeyboardState(NULL);

    // Reset ENTITY_IS_MOVING flag (in case no key is pressed).
    FLAG_CLEAR(pstBundle->pstSam->u16Flags, ENTITY_IS_MOVING);

    #ifndef __EMSCRIPTEN__
    if (u8KeyState[SDL_SCANCODE_Q])
    {
        _s32ExecStatus = EXIT_SUCCESS;
    }
    #endif

    if (u8KeyState[SDL_SCANCODE_0])
    {
        SetVideoZoomLevel(
            pstBundle->pstVideo,
            pstBundle->pstVideo->dZoomLevelInitial);
    }

    if (u8KeyState[SDL_SCANCODE_1])
    {
        pstBundle->pstVideo->dZoomLevel -= pstBundle->dDeltaTime;
        SetVideoZoomLevel(pstBundle->pstVideo, pstBundle->pstVideo->dZoomLevel);
    }

    if (u8KeyState[SDL_SCANCODE_2])
    {
        pstBundle->pstVideo->dZoomLevel += pstBundle->dDeltaTime;
        SetVideoZoomLevel(pstBundle->pstVideo, pstBundle->pstVideo->dZoomLevel);
    }

    if (u8KeyState[SDL_SCANCODE_LEFT])
    {
        FLAG_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_MOVING);
        FLAG_SET(pstBundle->pstSam->u16Flags, ENTITY_DIRECTION);
    }

    if (u8KeyState[SDL_SCANCODE_RIGHT])
    {
        FLAG_SET(pstBundle->pstSam->u16Flags,   ENTITY_IS_MOVING);
        FLAG_CLEAR(pstBundle->pstSam->u16Flags, ENTITY_DIRECTION);
    }

    // Set camera position.
    pstBundle->dCameraPosX =
        pstBundle->pstSam->dWorldPosX         -
        pstBundle->pstVideo->s32WindowWidth   /
        (pstBundle->pstVideo->dZoomLevel * 2) +
        (pstBundle->pstSam->u8Width      / 2);

    pstBundle->dCameraPosY =
        pstBundle->pstSam->dWorldPosY         -
        pstBundle->pstVideo->s32WindowHeight  /
        (pstBundle->pstVideo->dZoomLevel * 2) +
        (pstBundle->pstSam->u8Height     / 2);

    UpdateEntity(pstBundle->pstSam, pstBundle->dDeltaTime);

    if (FLAG_IS_NOT_SET(pstBundle->pstSam->u16Flags, ENTITY_DIRECTION))
    {
        for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
        {
            FLAG_SET(
                pstBundle->pstBG[u8Index]->u16Flags,
                BACKGROUND_SCROLL_DIRECTION);
        }
    }

    // Set camera boundaries to map size.
    pstBundle->dCameraMaxPosX = pstBundle->pstMap->u32Width -
        (pstBundle->pstVideo->s32WindowWidth  / pstBundle->pstVideo->dZoomLevel);
    pstBundle->dCameraMaxPosY = pstBundle->pstMap->u32Height -
        (pstBundle->pstVideo->s32WindowHeight / pstBundle->pstVideo->dZoomLevel);

    if (pstBundle->dCameraPosX < 0)
    {
        FLAG_SET(u16Flags, CAMERA_IS_LOCKED);
        pstBundle->dCameraPosX = 0;
    }
    else if (pstBundle->dCameraPosX > pstBundle->dCameraMaxPosX)
    {
        FLAG_SET(u16Flags, CAMERA_IS_LOCKED);
        pstBundle->dCameraPosX = pstBundle->dCameraMaxPosX;
    }
    else
    {
        FLAG_CLEAR(u16Flags, CAMERA_IS_LOCKED);
    }

    if (pstBundle->dCameraPosY < 0)
    {
        pstBundle->dCameraPosY = 0;
    }
    else if (pstBundle->dCameraPosY > pstBundle->dCameraMaxPosY)
    {
        pstBundle->dCameraPosY = pstBundle->dCameraMaxPosY;
    }

    // Scroll background if camera is not locked.
    if (FLAG_IS_NOT_SET(u16Flags, CAMERA_IS_LOCKED))
    {
        pstBundle->pstBG[4]->dVelocity = pstBundle->pstSam->dVelocityX  * pstBundle->dDeltaTime;
    }
    else
    {
        pstBundle->pstBG[4]->dVelocity = 0;
    }

    pstBundle->pstBG[3]->dVelocity = pstBundle->pstBG[4]->dVelocity / 2;
    pstBundle->pstBG[2]->dVelocity = pstBundle->pstBG[4]->dVelocity / 3;
    pstBundle->pstBG[1]->dVelocity = pstBundle->pstBG[4]->dVelocity / 4;
    pstBundle->pstBG[0]->dVelocity = pstBundle->pstBG[4]->dVelocity / 5;

    // Set sprite animation.
    if (FLAG_IS_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_IDLING))
    {
        SetEntitySpriteAnimation(pstBundle->pstSam, 0, 11, 0, 10);
    }
    if (FLAG_IS_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_IN_MID_AIR))
    {
        if (FLAG_IS_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_JUMPING))
        {
            SetEntitySpriteAnimation(pstBundle->pstSam, 14, 14, 0, 20);
        }
        else
        {
            /* If the entity is in mid air but isn't jumping, it is
             * falling downwards. */
            SetEntitySpriteAnimation(pstBundle->pstSam, 14, 14, 1, 20);
        }
    }
    if (FLAG_IS_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_MOVING))
    {
        SetEntitySpriteAnimation(pstBundle->pstSam, 0, 7, 1, 20);
    }
    FLAG_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_IDLING);

    // Set up collision detection.
    if (IsMapCoordOfType(
            pstBundle->pstMap,
            "Floor",
            pstBundle->pstSam->dWorldPosX + pstBundle->pstSam->u8Width,
            pstBundle->pstSam->dWorldPosY + pstBundle->pstSam->u8Height))
    {
        FLAG_CLEAR(pstBundle->pstSam->u16Flags, ENTITY_IS_IN_MID_AIR);
    }
    else
    {
        FLAG_SET(pstBundle->pstSam->u16Flags, ENTITY_IS_IN_MID_AIR);
    }

    #ifdef __EMSCRIPTEN__
    SDL_RenderClear(pstBundle->pstVideo->pstRenderer);
    #endif

    // Render scene.
    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        DrawBackground(
            pstBundle->pstVideo->pstRenderer,
            pstBundle->pstBG[u8Index],
            pstBundle->dCameraPosY);

            FLAG_CLEAR(
                pstBundle->pstBG[u8Index]->u16Flags,
                BACKGROUND_SCROLL_DIRECTION);
    }

    DrawMap(
        pstBundle->pstVideo->pstRenderer,
        pstBundle->pstMap,
        "Background",
        1,
        0,
        pstBundle->dCameraPosX,
        pstBundle->dCameraPosY);

    DrawEntity(
        pstBundle->pstVideo->pstRenderer,
        pstBundle->pstSam,
        pstBundle->dCameraPosX,
        pstBundle->dCameraPosY);

    DrawMap(
        pstBundle->pstVideo->pstRenderer,
        pstBundle->pstMap,
        "World",
        0,
        1,
        pstBundle->dCameraPosX,
        pstBundle->dCameraPosY);

    DrawMap(
        pstBundle->pstVideo->pstRenderer,
        pstBundle->pstMap,
        "Foreground",
        0,
        2,
        pstBundle->dCameraPosX,
        pstBundle->dCameraPosY);

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
    Background     *pstBG[5]  = { NULL };
    MainLoopBundle *pstBundle = NULL;
    Config          stConfig;
    Entity         *pstSam    = NULL;
    Map            *pstMap    = NULL;
    Video          *pstVideo  = NULL;

    if (s32ArgC > 1)
    {
        stConfig = InitConfig(pacArgV[1]);
    }
    else
    {
        #ifndef __EMSCRIPTEN__
        stConfig = InitConfig("default.ini");
        #else
        stConfig = InitConfig("emscripten.ini");
        #endif
    }

    pstVideo = InitVideo(
        "Boondock Sam",
        stConfig.stVideo.s32Width,
        stConfig.stVideo.s32Height,
        stConfig.stVideo.s8Fullscreen,
        1 + stConfig.stVideo.s32Height / 216); // 216 = Background height.
    if (NULL == pstVideo)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }
    atexit(SDL_Quit);

    pstMap = InitMap("res/maps/demo.tmx", "res/tilesets/jungle.png");
    if (NULL == pstMap)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }

    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        const char *pacBackgroundList[5] = {
            "res/backgrounds/plx-1.png",
            "res/backgrounds/plx-2.png",
            "res/backgrounds/plx-3.png",
            "res/backgrounds/plx-4.png",
            "res/backgrounds/plx-5.png"
        };

        pstBG[u8Index] = InitBackground(
            pstVideo->pstRenderer,
            pacBackgroundList[u8Index],
            pstVideo->s32WindowWidth);

        if (NULL == pstBG[u8Index])
        {
            _s32ExecStatus = EXIT_FAILURE;
            goto quit;
        }

        pstBG[u8Index]->dWorldPosY = pstMap->u32Height - pstBG[u8Index]->s32Height;
    }

    pstSam = InitEntity(24, 40, 264, 200, pstMap->u32Width);
    if (NULL == pstSam)
    {
        _s32ExecStatus = EXIT_FAILURE;
        goto quit;
    }
    if (-1 == LoadEntitySprite(pstSam, pstVideo->pstRenderer, "res/sprites/sam.png"))
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

    pstBundle->pstVideo       = pstVideo;
    pstBundle->pstMap         = pstMap;
    pstBundle->pstSam         = pstSam;
    pstBundle->dTimeA         = SDL_GetTicks();
    pstBundle->dCameraPosX    = 0;
    pstBundle->dCameraPosY    = 0;
    pstBundle->dCameraMaxPosX = 0;
    pstBundle->dCameraMaxPosY = 0;

    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        pstBundle->pstBG[u8Index] = pstBG[u8Index];
    }

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
    for (uint8_t u8Index = 0; u8Index < 5; u8Index++)
    {
        free(pstBG[u8Index]);
    }
    FreeMap(pstMap);
    free(pstSam);
    free(pstBundle);
    TerminateVideo(pstVideo);

    return _s32ExecStatus;
}
