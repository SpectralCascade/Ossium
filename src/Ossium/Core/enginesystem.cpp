#include "window.h"
#include "enginesystem.h"

namespace Ossium
{

    EngineSystem::~EngineSystem()
    {
        delete ecs;
        delete services;
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
        bool quit = false;

        Renderer* renderer = services->GetService<Renderer>();

        /// Input handling phase
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            WindowManager* windows = services->GetService<WindowManager>();
            if (windows != nullptr)
            {
                Window* closed = windows->HandleEvent(currentEvent);
                if (closed != nullptr)
                {
                    /// TODO: Check if the window is the main engine window?
                    quit = true;
                    break;
                }
            }
            else
            {
                /// If no window manager is available, try and acquire the main window via the main renderer service.
                if (renderer != nullptr)
                {
                    if (renderer->GetWindow()->HandleEvent(currentEvent) < 0)
                    {
                        quit = true;
                        break;
                    }
                }
            }
            if (currentEvent.type == SDL_QUIT
                #ifdef DEBUG
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

}
