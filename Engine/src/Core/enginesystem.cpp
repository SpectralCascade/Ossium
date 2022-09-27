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
#include "ecs.h"
#include "../Components/UI/LayoutSurface.h"

namespace Ossium
{

    EngineSystem::EngineSystem(const Config& config)
    {
        window = new Window(config.windowTitle.c_str(), config.windowWidth, config.windowHeight, config.fullscreen, config.windowFlags);
        renderer = new Renderer(window, config.totalRenderLayers, -1, true);
        input = new InputController();
        services = new ServicesProvider(renderer, &resources, input);
        Init(config);
    }

    EngineSystem::~EngineSystem()
    {
        delete services;
        delete input;
        delete renderer;
        delete window;
    }

    void EngineSystem::Init(const Config& config)
    {
        delta.Init(config.fpscap);
        for (std::string scenePath : config.startScenes)
        {
            if (!resources.LoadAndInit<Scene>(scenePath, services))
            {
                Log.Warning("Failed to load start scene \"{0}\"!", scenePath);
            }
        }
    }

    bool EngineSystem::Update()
    {
        bool quit = doExit;
        if (quit)
        {
            return false;
        }

        // Input handling phase
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            if (currentEvent.type == SDL_QUIT || (SDL_GetModState() == KMOD_LALT && (currentEvent.type == SDL_KEYDOWN && currentEvent.key.keysym.sym == SDLK_F4)))
            {
                quit = true;
                break;
            }
            window->HandleEvent(currentEvent);
            input->HandleEvent(currentEvent);
        }

        // Update services before main logic update.
        services->PreUpdate();

        // Update game logic in loaded scenes
        for (auto itr : resources.GetAll<Scene>())
        {
            ((Scene*)itr.second)->UpdateComponents();
        }

        // Update services after the main logic update.
        services->PostUpdate();

        SDL_RenderClear(renderer->GetRendererSDL());

        for (auto itr : resources.GetAll<Scene>())
        {
            Scene* scene = (Scene*)itr.second;
            // Update all layouts now everything has moved.
            scene->WalkEntities([=] (Entity* entity) {
                if (entity->IsActive())
                {
                    LayoutSurface* layoutSurface = entity->GetComponent<LayoutSurface>();
                    if (layoutSurface && layoutSurface->IsEnabled())
                    {
                        layoutSurface->LayoutUpdate();
                    }
                    return true;
                }
                return false;
            });

            // Finally, render the scene.
            scene->Render();
        }

        // Render everything
        renderer->RenderPresent(true);
        SDL_RenderPresent(renderer->GetRendererSDL());

        // Destroy entities and components pending destruction.
        for (auto itr : resources.GetAll<Scene>())
        {
            ((Scene*)itr.second)->DestroyPending();
        }

        // Update services post-render
        services->PostRender();

        // Update the engine time.
        delta.Update();

        return !quit;
    }

    void EngineSystem::Exit()
    {
        doExit = true;
    }

}
