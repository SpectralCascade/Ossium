/** COPYRIGHT NOTICE
 *  
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
#include <Ossium.h>
#include "Components/editorview.h"

using namespace Ossium;
using namespace Ossium::Editor;

/// Main entry point for the program.
int main(int argc, char* argv[])
{

    Logger log;

    if (InitialiseOssium() >= 0)
    {
        // Setup main window
        Window window("Ossium Editor", 640, 480, false, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

        // Setup services
        Renderer renderer(&window);
        ResourceController resources;
        InputController input;
        InputContext mainContext;
        input.AddContext("main", &mainContext);

        ServicesProvider services(&renderer, &resources, &input);

        //EngineSystem engine(&services);

        EditorWindow view(&renderer, &mainContext, &resources);
        //view.OnGUI();

        SDL_Event currentEvent;

        bool quit = false;
        while (!quit)//engine.Update())
        {
            /// Input handling phase
            while (SDL_PollEvent(&currentEvent) != 0)
            {
                if (window.HandleEvent(currentEvent) < 0)
                {
                    quit = true;
                    break;
                }
                if (currentEvent.type == SDL_QUIT
                    #ifdef OSSIUM_DEBUG
                    || (currentEvent.type == SDL_KEYUP && currentEvent.key.keysym.sym == SDLK_ESCAPE)
                    #endif // DEBUG
                ) {
                    quit = true;
                    break;
                }
                input.HandleEvent(currentEvent);
            }

        }

    }
    else
    {
        log.Error("Failed to initialise Ossium.");
    }

    TerminateOssium();
}
