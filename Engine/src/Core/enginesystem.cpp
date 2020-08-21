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
#include "../Components/collider.h"

namespace Ossium
{

    void EngineSystem::Init(const Config& config)
    {
        delta.Init(config.fpscap);
        if (!config.startScene.empty())
        {
            if (!resources.LoadAndInit<Scene>(config.startScene, &services))
            {
                Log.Warning("Failed to load start scene \"{0}\"!", config.startScene);
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
            if (currentEvent.type == SDL_QUIT
#ifdef OSSIUM_DEBUG
                || (SDL_GetModState() == KMOD_LALT && (currentEvent.type == SDL_KEYDOWN && currentEvent.key.keysym.sym == SDLK_F4))
#endif // DEBUG
            ) {
                quit = true;
                break;
            }
            window.HandleEvent(currentEvent);
            InputController* input = services.GetService<InputController>();
            if (input != nullptr)
            {
                input->HandleEvent(currentEvent);
            }
        }

        // Update services before main logic update.
        services.PreUpdate();

        // Update game logic in loaded scenes
        for (auto itr : resources.GetAll<Scene>())
        {
            ((Scene*)itr.second)->UpdateComponents();
        }

        // Update services after the main logic update.
        services.PostUpdate();

        // TODO: Optimise this? Perhaps there's a way transforms can point directly to the physics world transform when a physics body is associated.
        // Once the physics world has updated, update all the transforms associated with physics bodies.
        for (auto itr : resources.GetAll<Scene>())
        {
            ((Scene*)itr.second)->WalkComponents<PhysicsBody>([](PhysicsBody* body){ body->UpdatePhysics(); });
        }

        // Render everything
        renderer.RenderPresent();

        // Destroy entities and components pending destruction.
        for (auto itr : resources.GetAll<Scene>())
        {
            ((Scene*)itr.second)->DestroyPending();
        }

        // Update services post-render
        services.PostRender();

        // Update the engine time.
        delta.Update();

        return !quit;
    }

    void EngineSystem::Exit()
    {
        doExit = true;
    }

}
