#include "window.h"
#include "enginesystem.h"

namespace Ossium
{

    EngineSystem::~EngineSystem()
    {
        delete ecs;
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

        /// Input handling phase
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            ecs->GetService<Renderer>()->GetWindow()->HandleEvents(currentEvent);
            if (currentEvent.type == SDL_QUIT
                #ifdef DEBUG
                || (currentEvent.type == SDL_KEYUP && currentEvent.key.keysym.sym == SDLK_ESCAPE)
                #endif // DEBUG
            ) {
                quit = true;
                break;
            }
            input.HandleEvent(currentEvent);
        }

        /// General game logic update
        ecs->UpdateComponents();

        /// Render everything
        ecs->GetService<Renderer>()->RenderPresent();

        /// Destroy entities and components that are pending destruction
        /// now we've finished rendering.
        ecs->DestroyPending();

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
