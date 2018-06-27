/** @file Config.h
 * @ingroup Config
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

/**
 * @ingroup Config
 */
typedef struct VideoConfig_t {
    int32_t s32Height;
    int32_t s32Width;
    int8_t  s8Fullscreen;
    int8_t  s8LimitFPS;
    int8_t  s8FPS;
} VideoConfig;

/**
 * @ingroup Config
 */
typedef struct Config_t {
    VideoConfig stVideo;
} Config;

Config InitConfig(const char *pcFilename);

#endif
