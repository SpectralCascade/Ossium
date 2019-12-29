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
