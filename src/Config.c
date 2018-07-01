/**
 * @file      config.c
 * @ingroup   Config
 * @defgroup  Config
 * @brief     Configuration file manager.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include "inih/ini.h"

static int32_t _Handler(
    void* pConfig,
    const char *pacSection,
    const char *pacName,
    const char *pacValue)
{
    Config  *pstConfig = (Config*)pConfig;
    int32_t  s32Value  = atoi(pacValue);

    #define MATCH(pacS, pacN) strcmp(pacSection, pacS) == 0 && strcmp(pacName, pacN) == 0

    if      (MATCH("Video", "width"))      { pstConfig->stVideo.s32Width     = s32Value; }
    else if (MATCH("Video", "height"))     { pstConfig->stVideo.s32Height    = s32Value; }
    else if (MATCH("Video", "fullscreen")) { pstConfig->stVideo.s8Fullscreen = s32Value; }
    else if (MATCH("Video", "fps"))        { pstConfig->stVideo.s8FPS        = s32Value; }
    else if (MATCH("Video", "limitFPS"))   { pstConfig->stVideo.s8LimitFPS   = s32Value; }
    else
    {
        return 0;
    }

    return 1;
}

/**
 * @brief
 * @param
 * @return
 * @ingroup Config
 */
Config InitConfig(const char *pacFilename)
{
    static Config stConfig;

    stConfig.stVideo.s32Width      = 800;
    stConfig.stVideo.s32Height     = 600;
    stConfig.stVideo.s8Fullscreen  =   0;
    stConfig.stVideo.s8FPS         =  60;
    stConfig.stVideo.s8LimitFPS    =   1;

    if (0 > ini_parse(pacFilename, _Handler, &stConfig))
    {
        fprintf(stderr, "Couldn't load configuration file: %s\n", pacFilename);
    }

    if (0 > stConfig.stVideo.s8FPS)     { stConfig.stVideo.s8FPS     = abs(stConfig.stVideo.s8FPS);     }
    if (0 > stConfig.stVideo.s32Height) { stConfig.stVideo.s32Height = abs(stConfig.stVideo.s32Height); }
    if (0 > stConfig.stVideo.s32Width)  { stConfig.stVideo.s32Width  = abs(stConfig.stVideo.s32Width);  }

    return stConfig;
}
