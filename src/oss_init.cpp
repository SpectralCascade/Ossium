#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "oss_init.h"

int OSS_Init()
{
    /// Ensure errors are output to console if debug build (use "-D DEBUG" in GCC compile options)
    #ifdef DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
    #endif // DEBUG

    int error = 0;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        error = OSS_INIT_ERROR_SDL;
    }
    else
    {
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, IMG_GetError());
            error = OSS_INIT_ERROR_IMG;
        }
        else
        {
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, Mix_GetError());
                error = OSS_INIT_ERROR_MIXER;
            }
            else
            {
                /// Change this number of channels later as necessary
                Mix_AllocateChannels(16);
                if (TTF_Init() == -1)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
                    error = OSS_INIT_ERROR_TTF;
                }
                else
                {
                    SDL_Log("Initialised OSSIUM ENGINE successfully!");
                }
            }
        }
    }
    return error;
}

SDL_Renderer* OSS_CreateRenderer(SDL_Window* window, bool vsync)
{
    if (window == NULL)
    {
        return NULL;
    }
    if (vsync)
    {
        return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
    else
    {
        return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
}
