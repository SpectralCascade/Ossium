#include "enginesystem.h"

namespace Ossium
{

    EngineSystem::EngineSystem(Renderer* graphicsRenderer, JSON& configData, Window* engineWindow)
    {
        renderer = graphicsRenderer;
        window = engineWindow;
        Init(configData);
    }

    EngineSystem::EngineSystem(Renderer* graphicsRenderer, string configFilePath, Window* engineWindow)
    {
        renderer = graphicsRenderer;
        window = engineWindow;
        if (!configFilePath.empty())
        {
            Init(configFilePath);
        }
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
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to load EngineSystem initialisation file '%s'!", configFilePath.c_str());
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
            if (window != nullptr)
            {
                window->HandleEvents(currentEvent);
            }
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
        ecs.UpdateComponents();

        /// Render everything
        renderer->RenderPresent();

        /// Destroy entities and components that are pending destruction
        /// now we've finished rendering.
        ecs.DestroyPending();

        delta.Update();

        return !quit;
    }

}
