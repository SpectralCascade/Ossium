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
#ifndef ENGINESYSTEM_H
#define ENGINESYSTEM_H

#include "input.h"
#include "delta.h"
#include "component.h"

namespace Ossium
{

    class OSSIUM_EDL EngineSystem : public GlobalServices
    {
    public:
        friend class Scene;

        template<typename ...Args>
        EngineSystem(ServicesProvider* engineServices, JSON& configData)
        {
            services = engineServices;
            ecs = new Scene(services);
            Init(configData);
        }

        template<typename ...Args>
        EngineSystem(ServicesProvider* engineServices, string configFilePath = "")
        {
            services = engineServices;
            ecs = new Scene(services);
            if (!configFilePath.empty())
            {
                Init(configFilePath);
            }
        }

        ~EngineSystem();

        /// Configures the engine with a JSON file.
        void Init(string configFilePath);
        /// Configures the engine with a JSON object.
        void Init(JSON& configData);

        /// Executes the main game loop, including input handling and rendering.
        /// Returns false when the game should stop, otherwise returns true.
        bool Update();

        /// Loads a game scene into the entity component system.
        bool LoadScene(string path);

        /// Returns the ECS instance.
        Scene* GetScene();

        /// Returns the services provider.
        ServicesProvider* GetServices();

    private:
        NOCOPY(EngineSystem);

        /// The core entity-component system.
        Scene* ecs = nullptr;

        /// Services available to this engine system instance.
        ServicesProvider* services = nullptr;

        /// SDL_Event for input handling.
        SDL_Event currentEvent;

        /// Time keeping for this system.
        Delta delta;

    };

}

#endif // ENGINESYSTEM_H
