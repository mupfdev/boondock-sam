/**
 * @file Audio.c
 * @ingroup   Audio
 * @defgroup  Audio
 * @brief     Audio handler to playback music and sound effects.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdint.h>
#include <stdio.h>
#include "Audio.h"

/**
 * @brief   Play music in using a fade-in effect.
 * @param   pstMusic    the Music.  See @ref struct Music.
 * @param   s8Loops     number of loops, -1 plays the music forever.
 * @param   u16TimeInMS time in milliseconds to fade-in music.
 * @return  0 on success, -1 on error.
 * @ingroup Audio
 */
int8_t FadeInMusic(
    Music    *pstMusic,
    int8_t    s8Loops,
    uint16_t  u16TimeInMS)
{
    if (-1 == Mix_FadeInMusic(pstMusic->pstMusic, s8Loops, u16TimeInMS))
    {
        fprintf(stderr, "%s\n", Mix_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief   Free Mixer from memory.
 * @param   a Mixer.  See @ref struct Mixer.
 * @ingroup Audio
 */
void FreeMixer(Mixer *pstMixer)
{
    free(pstMixer);
    Mix_CloseAudio();
    while(Mix_Init(0)) Mix_Quit();
}

/**
 * @brief   Initialise Mixer.
 * @return  Mixer on success, NULL on error.  See @ref struct Mixer.
 * @ingroup Audio
 */
Mixer *InitMixer()
{
    static Mixer *pstMixer;
    pstMixer = malloc(sizeof(struct Mixer_t));
    if (NULL == pstMixer)
    {
        fprintf(stderr, "InitMixer(): error allocating memory.\n");
        return NULL;
    }

    if (-1 == SDL_Init(SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Couldn't initialise SDL: %s\n", SDL_GetError());
        free(pstMixer);
        return NULL;
    }

    pstMixer->u16AudioFormat       = MIX_DEFAULT_FORMAT;
    pstMixer->u16ChunkSize         = 4096;
    pstMixer->u8NumChannels        = 2;
    pstMixer->u16SamplingFrequency = 44100;

    if (-1 == Mix_OpenAudio(
            pstMixer->u16SamplingFrequency,
            pstMixer->u16AudioFormat,
            pstMixer->u8NumChannels,
            pstMixer->u16ChunkSize))
    {
        fprintf(stderr, "%s\n", Mix_GetError());
        free(pstMixer);
        return NULL;
    }
    Mix_AllocateChannels(16);

    return pstMixer;
}

/**
 * @brief   Initialise Music.
 * @param   pacFilename the filename of the ogg music file.
 * @return  Music on success, NULL on error.  See @ref struct Music.
 * @ingroup Audio
 */
Music *InitMusic(const char *pacFilename)
{
    static Music *pstMusic;
    pstMusic = malloc(sizeof(struct Music_t));
    if (NULL == pstMusic)
    {
        fprintf(stderr, "InitMusic(): error allocating memory.\n");
        return NULL;
    }

    pstMusic->pstMusic = Mix_LoadMUS(pacFilename);

    if (NULL == pstMusic->pstMusic)
    {
        fprintf(stderr, "%s\n", Mix_GetError());
        free(pstMusic);
        return NULL;
    }

    return pstMusic;
}

/**
 * @brief   Initialise Sfx.
 * @param   pacFilename the filename of the wav sound effect.
 * @return  Sfx on success, NULL on error.
 * @ingroup Audio
 */
Sfx *InitSfx(const char *pacFilename)
{
    static Sfx *pstSfx;
    pstSfx = malloc(sizeof(struct Sfx_t));
    if (NULL == pstSfx)
    {
        fprintf(stderr, "InitSfx(): error allocating memory.\n");
        return NULL;
    }

    pstSfx->pstSfx = Mix_LoadWAV(pacFilename);

    if (NULL == pstSfx->pstSfx)
    {
        fprintf(stderr, "%s\n", Mix_GetError());
        free(pstSfx);
        return NULL;
    }

    return pstSfx;
}

/**
 * @brief   Play Music.
 * @param   pstMusic the Music.  See @ref struct Music.
 * @param   s8Loops  number of loops, -1 plays the music forever.
 * @return  0 on success, -1 on error.
 * @ingroup Audio
 */
int8_t PlayMusic(Music *pstMusic, int8_t s8Loops)
{
    if (-1 == Mix_PlayMusic(pstMusic->pstMusic, s8Loops))
    {
        fprintf(stderr, "%s\n", Mix_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief   Play Sfx.
 * @param   pstSfx    a Sfx.  See @ref struct Sfx.
 * @param   s8Channel the mixer channel to use.
 * @param   s8Loops   number of loops, -1 plays the effect forever.
 * @return  0 on success, -1 on error.
 * @ingroup Audio
 */
int8_t PlaySfx(
    Sfx    *pstSfx,
    int8_t  s8Channel,
    int8_t  s8Loops)
{
    if (0 == Mix_Playing(s8Channel))
    {
        if (-1 == Mix_PlayChannel(s8Channel, pstSfx->pstSfx, s8Loops))
        {
            fprintf(stderr, "%s\n", Mix_GetError());
            return -1;
        }
    }

    return 0;
}

/**
 * @brief   Toggle Music playback.
 * @ingroup Audio
 */
void ToggleMusic()
{
    if (Mix_PausedMusic())
    {
        Mix_ResumeMusic();
    }
    else
    {
        Mix_PauseMusic();
    }
}
