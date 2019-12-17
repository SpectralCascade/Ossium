#include <cstdio>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "init.h"
#include "delta.h"
#include "ecs.h"
#include "audio.h"

using namespace std;

namespace Ossium
{
    int InitialiseOssium()
    {
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

        string numCPUs = "";
        Logger::EngineLog().Info("{0} | {1} core CPU | {2} MB memory\n", SDL_GetPlatform(), SDL_GetCPUCount(), SDL_GetSystemRAM());

        int error = 0;
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            Logger::EngineLog().Error("SDL error during initialisation: {0}", SDL_GetError());
            error = INIT_ERROR_SDL;
        }
        else
        {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                Logger::EngineLog().Error("SDL_Image error during initialisation: {0}", IMG_GetError());
                error = INIT_ERROR_IMG;
            }
            else
            {
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    Logger::EngineLog().Error("SDL_Mixer error during initialisation: {0}", Mix_GetError());
                    error = INIT_ERROR_MIXER;
                }
                else
                {
                    /// Initialise the Audio channel subsystem
                    Audio::Internals::ChannelController::_Instance().Init(50);

                    #ifdef _SDL_TTF_H
                    if (TTF_Init() == -1)
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
                        error = INIT_ERROR_TTF;
                    }
                    else
                    {
                        Logger::EngineLog().Info("Initialised OSSIUM ENGINE successfully!");
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
        SDL_Quit();
        printf("INFO: Successfully terminated Ossium.\n");
    }

}
