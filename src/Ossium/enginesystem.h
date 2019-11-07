#ifndef ENGINESYSTEM_H
#define ENGINESYSTEM_H

#include "input.h"
#include "delta.h"
#include "component.h"

namespace Ossium
{

    class EngineSystem : public GlobalServices
    {
    public:
        friend class EntityComponentSystem;

        EngineSystem(Renderer* graphicsRenderer, JSON& configData);
        EngineSystem(Renderer* graphicsRenderer, string configFilePath = "");
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
