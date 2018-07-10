/**
 * @file Audio.h
 * @ingroup Audio
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <SDL2/SDL_mixer.h>
#include <stdint.h>

/**
 * @ingroup Audio
 */
typedef struct Mixer_t {
    uint16_t u16AudioFormat;
    uint16_t u16ChunkSize;
    uint8_t  u8NumChannels;
    uint16_t u16SamplingFrequency;
} Mixer;

/**
 * @ingroup Audio
 */
typedef struct Music_t {
    Mix_Music *pstMusic;
} Music;

/**
 * @ingroup Audio
 */
typedef struct Sfx_t {
    Mix_Chunk *pstSfx;
} Sfx;

int8_t FadeInMusic(
    Music    *pstMusic,
    int8_t    s8Loops,
    uint16_t  u16TimeInMS);

void   FreeMixer(Mixer *pstMixer);
Mixer *InitMixer();
Music *InitMusic(const char *pacFilename);
Sfx   *InitSfx(const char *pacFilename);

int8_t PlayMusic(
    Music  *pstMusic,
    int8_t  s8Loops);

int8_t PlaySfx(
    Sfx    *pstSfx,
    int8_t  s8Channel,
    int8_t  s8Loops);

void ToggleMusic();

#endif // _AUDIO_H_
