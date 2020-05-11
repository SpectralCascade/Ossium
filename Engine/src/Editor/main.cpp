#include "../Ossium.h"
#include "Examples/simple_demo_window.h"
#include "Examples/demo_window_docking.h"

using namespace Ossium;
using namespace Ossium::Editor;
using namespace std;

int main(int argc, char* argv[])
{
    InitialiseOssium();

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    // Setup input
    InputController input;
    // Window contexts
    InputContext mainContext;
    input.AddContext("MainWindow", &mainContext);

    // Setup resources
    ResourceController resources;

    // The main native window manager that deals with editor window docking
    NativeEditorWindow window(&input, &resources);
    window.Add<DemoDockingWindow>(DockingMode::TOP);

    //window.Insert(&dockView, &view, DockingMode::LEFT);

    bool quit = false;

    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE))
            {
                quit = true;
                break;
            }
            input.HandleEvent(e);
        }

        // Update the GUI
        window.Update();
    }

    resources.FreeAll();

    TerminateOssium();
    return 0;
}
