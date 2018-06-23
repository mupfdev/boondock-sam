/** @file Video.h
 * @ingroup Video
 */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <SDL2/SDL.h>
#include <stdint.h>

/**
 * @ingroup Video
 */
typedef struct Video_t
{
    SDL_Renderer *stRenderer;
    SDL_Window   *stWindow;
    int32_t       s32WindowHeight;
    int32_t       s32WindowWidth;
    double        dZoomLevel;
    double        dZoomLevelInital;
} Video;

Video *InitVideo(
    const char    *pcTitle,
    const int32_t  s32Width,
    const int32_t  s32Height,
    const uint8_t  u8Fullscreen,
    const double   dZoomLevel);

int8_t SetVideoZoomLevel(Video *stVideo, double dZoomLevel);
void   TerminateVideo(Video *stVideo);
void   UpdateVideo(SDL_Renderer *stRenderer);

#endif
