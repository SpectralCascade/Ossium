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

    // Setup resources
    ResourceController resources;

    // The main native window manager that deals with editor window docking
    NativeEditorWindow* window = new NativeEditorWindow(&input, &resources, "Ossium");
    window->Add<DemoDockingWindow>(DockingMode::TOP);

    //window.Insert(&dockView, &view, DockingMode::LEFT);

    bool quit = false;

    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            window->HandleEvent(e);
            input.HandleEvent(e);
        }

        // Update the GUI
        window->Update();
    }

    resources.FreeAll();

    TerminateOssium();
    return 0;
}
