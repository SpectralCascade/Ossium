#include <cstdio>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "init.h"
#include "delta.h"
#include "ecs.h"

using namespace std;

namespace ossium
{
    int InitialiseOssium()
    {
        float processorSpeed = (float)SDL_GetPerformanceFrequency() / 1000000000;
        string numCPUs = "";
        switch (SDL_GetCPUCount())
        {
        case 1:
            numCPUs = "Single-core";
            break;
        case 2:
            numCPUs = "Dual-core";
            break;
        case 4:
            numCPUs = "Quad-core";
            break;
        case 6:
            numCPUs = "Hexa-core";
            break;
        case 8:
            numCPUs = "Octa-core";
            break;
        case 10:
            numCPUs = "Deca-core";
            break;
        default:
            numCPUs = "Unknown";
            break;
        }
        SDL_Log("%s | %s CPU at %f GHz | %d MB memory\n", SDL_GetPlatform(), numCPUs.c_str(), processorSpeed, SDL_GetSystemRAM());

        /// Initialise the ECS subsystem
        ecs::InitECS();

        /// Ensure errors are output to console if debug build (use "-D DEBUG" in GCC compile options)
        #ifdef DEBUG
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
        #endif // DEBUG

        int error = 0;
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL error during initialisation: %s", SDL_GetError());
            error = INIT_ERROR_SDL;
        }
        else
        {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Image error during initialisation: %s", IMG_GetError());
                error = INIT_ERROR_IMG;
            }
            else
            {
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Mixer error during initialisation: %s", Mix_GetError());
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

    void TerminateOssium()
    {
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        ecs::DestroyECS();
        SDL_Quit();
        printf("INFO: Successfully terminated Ossium.");
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
