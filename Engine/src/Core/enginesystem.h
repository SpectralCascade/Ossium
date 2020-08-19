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
#include "config.h"

namespace Ossium
{

    class OSSIUM_EDL EngineSystem
    {
    public:
        friend class Scene;

        template<typename ...Args>
        EngineSystem(const Config& config = {}, Args ...serviceList) :
            window(config.windowTitle.c_str(), config.windowWidth, config.windowHeight, config.fullscreen, config.windowFlags),
            renderer(&window, config.totalRenderLayers, (config.vsync ? SDL_RENDERER_PRESENTVSYNC : 0) | (config.hardwareAcceleration ? SDL_RENDERER_ACCELERATED : 0)),
            services(&renderer, &resources, std::forward<Args>(serviceList)...)
        {
            Init(config);
        }

        /// Configures the engine.
        void Init(const Config& config);

        /// Executes the main game loop, including input handling and rendering.
        /// Returns false when the game should stop, otherwise returns true.
        bool Update();

        /// Indicates that the engine should return false on the next Update() call.
        void Exit();

    private:
        NOCOPY(EngineSystem);

        /// Should the engine return false at the end of the next Update() call?
        bool doExit = false;

        /// Native window.
        Window window;

        /// Main renderer associated with the window.
        Renderer renderer;

        /// Services available to this engine system instance. Always provides a renderer at the very least.
        ServicesProvider services;

        /// Resource controller for all resources used by the game, including scenes, images, sounds etc.
        ResourceController resources;

        /// SDL_Event for input handling.
        SDL_Event currentEvent;

        /// Time keeping for this system.
        Delta delta;

    };

}

#endif // ENGINESYSTEM_H
