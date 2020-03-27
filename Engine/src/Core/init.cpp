/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
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

        /// Initialise ECS subsystem
        if (TypeFactory<BaseComponent, ComponentType>::Init() < 0)
        {
            Logger::EngineLog().Warning("Failed to initialise ECS subsystem, component polymorphism not supported!");
        }

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
                    Audio::Internals::ChannelController::Instance.Init(50);

                    if (TTF_Init() == -1)
                    {
                        Logger::EngineLog().Error("SDL_ttf error during initialisation: {0}", TTF_GetError());
                        error = INIT_ERROR_TTF;
                    }
                    else
                    {
                        Logger::EngineLog().Info("Initialised OSSIUM ENGINE successfully!");
                    }
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
