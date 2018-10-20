#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "init.h"

namespace ossium
{
    int Init()
    {
        float processorSpeed = (float)SDL_GetPerformanceFrequency() / 1000000000;
        char* numCPUs = NULL;
        switch (SDL_GetCPUCount())
        {
        case 1:
            numCPUs = new char[11];
            numCPUs = "Single-core";
            break;
        case 2:
            numCPUs = new char[9];
            numCPUs = "Dual-core";
            break;
        case 4:
            numCPUs = new char[9];
            numCPUs = "Quad-core";
            break;
        case 6:
            numCPUs = new char[9];
            numCPUs = "Hexa-core";
            break;
        case 8:
            numCPUs = new char[9];
            numCPUs = "Octa-core";
            break;
        case 10:
            numCPUs = new char[9];
            numCPUs = "Deca-core";
            break;
        default:
            numCPUs = new char[7];
            numCPUs = "Unknown";
            break;
        }
        SDL_Log("%s | %s CPU at %f GHz | %d MB memory\n", SDL_GetPlatform(), numCPUs, processorSpeed, SDL_GetSystemRAM());

        /// Ensure errors are output to console if debug build (use "-D DEBUG" in GCC compile options)
        #ifdef DEBUG
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
        #endif // DEBUG

        int error = 0;
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
            error = INIT_ERROR_SDL;
        }
        else
        {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, IMG_GetError());
                error = INIT_ERROR_IMG;
            }
            else
            {
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, Mix_GetError());
                    error = INIT_ERROR_MIXER;
                }
                else
                {
                    /// Change this number of channels later as necessary
                    Mix_AllocateChannels(16);
                    #ifdef _SDL_TTF_H
                    if (TTF_Init() == -1)
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
                        error = INIT_ERROR_TTF;
                    }
                    else
                    {
                        SDL_Log("Initialised OSSIUM ENGINE successfully!");
                    }
                    #endif // _SDL_TTF_H
                }
            }
        }
        return error;
    }

    SDL_Renderer* CreateRenderer(SDL_Window* window, bool vsync)
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

}
