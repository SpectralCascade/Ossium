#ifndef ENGINESYSTEM_H
#define ENGINESYSTEM_H

#include "ecs.h"
#include "resourcecontroller.h"
#include "input.h"
#include "renderer.h"
#include "window.h"
#include "delta.h"

namespace Ossium
{

    class EngineSystem
    {
    public:
        friend class EntityComponentSystem;

        EngineSystem(Renderer* graphicsRenderer, JSON& configData, Window* engineWindow = nullptr);
        EngineSystem(Renderer* graphicsRenderer, string configFilePath = "", Window* engineWindow = nullptr);
        ~EngineSystem() = default;

        /// Configures the engine with a JSON file.
        void Init(string configFilePath);
        /// Configures the engine with a JSON object.
        void Init(JSON& configData);

        /// Executes the main game loop, including input handling and rendering.
        /// Returns false when the game should stop, otherwise returns true.
        bool Update();

    private:
        NOCOPY(EngineSystem);

        /// The core entity-component system.
        EntityComponentSystem ecs;

        /// The graphics renderer.
        Renderer* renderer = nullptr;

        /// The main game window, if any.
        Window* window = nullptr;

        /// All resources by file path.
        ResourceController resources;

        /// The main input controller.
        InputController input;

        /// SDL_Event for input handling.
        SDL_Event currentEvent;

        /// Time keeping for this system.
        Delta delta;

    };

}

#endif // ENGINESYSTEM_H
