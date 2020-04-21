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
#include "window.h"
#include "enginesystem.h"

namespace Ossium
{

    EngineSystem::~EngineSystem()
    {
        delete ecs;
    }

    void EngineSystem::ClearScene()
    {
        delete ecs;
        ecs = new Scene(services);
    }

    void EngineSystem::Init(JSON& configData)
    {
        /// Set flags here to configure how the engine operates.
    }

    void EngineSystem::Init(string configFilePath)
    {
        /// Load the configuration file into a JSON object
        JSON data;
        if (!data.Import(configFilePath))
        {
            Logger::EngineLog().Warning("Failed to load EngineSystem initialisation file '{0}'!", configFilePath);
            return;
        }
        /// Initialise as normal
        Init(data);
    }

    bool EngineSystem::Update()
    {
        bool quit = doExit;
        if (doExit)
        {
            return false;
        }

        Renderer* renderer = services->GetService<Renderer>();

        /// Input handling phase
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            if (currentEvent.type == SDL_QUIT
                #ifdef OSSIUM_DEBUG
                || (currentEvent.type == SDL_KEYUP && currentEvent.key.keysym.sym == SDLK_ESCAPE)
                #endif // DEBUG
            ) {
                quit = true;
                break;
            }
            InputController* input = services->GetService<InputController>();
            if (input != nullptr)
            {
                input->HandleEvent(currentEvent);
            }
        }

        /// Update services before main logic update.
        services->PreUpdate();

        /// General game logic update
        ecs->UpdateComponents();

        /// Update services after the main logic update.
        services->PostUpdate();

        if (renderer != nullptr)
        {
            /// Render everything
            renderer->RenderPresent();
        }

        /// Destroy entities and components that are pending destruction
        /// now we've finished rendering.
        ecs->DestroyPending();

        /// Update services post-render
        services->PostRender();

        /// Update the engine time.
        delta.Update();

        return !quit;
    }

    bool EngineSystem::LoadScene(string path)
    {
        JSON raw;
        if (raw.Import(path))
        {
            string data = raw.ToString();
            ecs->FromString(data);
            return true;
        }
        return false;
    }

    Scene* EngineSystem::GetScene()
    {
        return ecs;
    }

    ServicesProvider* EngineSystem::GetServices()
    {
        return services;
    }

    void EngineSystem::Exit()
    {
        doExit = true;
    }

}
