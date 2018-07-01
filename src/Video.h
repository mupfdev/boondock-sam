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
enum VideoLimits
{
    VIDEO_MIN_ZOOMLEVEL = 1,
    VIDEO_MAX_ZOOMLEVEL = 4
};

/**
 * @ingroup Video
 */
typedef struct Video_t
{
    SDL_Renderer *pstRenderer;
    SDL_Window   *pstWindow;
    int32_t       s32WindowHeight;
    int32_t       s32WindowWidth;
    double        dZoomLevel;
    double        dZoomLevelInitial;
} Video;

Video *InitVideo(
    const char    *pacTitle,
    const int32_t  s32Width,
    const int32_t  s32Height,
    const uint8_t  u8Fullscreen,
    const double   dZoomLevel);

int8_t SetVideoZoomLevel(Video *pstVideo, double dZoomLevel);
void   TerminateVideo(Video *pstVideo);
void   UpdateVideo(SDL_Renderer *pstRenderer);

#endif
