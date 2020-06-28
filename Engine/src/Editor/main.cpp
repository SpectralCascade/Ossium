#include "../Ossium.h"
#include "Examples/simple_demo_window.h"
#include "Examples/demo_window_docking.h"
#include "Examples/layout_diagram.h"
#include "Core/contextmenu.h"

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
    EditorLayout* window = new EditorLayout(&input, &resources, "Ossium");
    //EditorLayout* layoutView = new EditorLayout(&input, &resources, "Layout Tree");
    window->Add<DemoDockingWindow>(DockingMode::LEFT);
    //layoutView->Add<LayoutDiagram>(DockingMode::LEFT)->target = window;

    //window.Insert(&dockView, &view, DockingMode::LEFT);

    bool quit = false;

    window->GetNativeWindow()->OnCloseButton += [&] (Window& caller) {
        quit = true;
    };

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
            ContextMenu::HandleInput(e);
            //layoutView->HandleEvent(e);
            input.HandleEvent(e);
        }

        // Update the GUI
        window->Update();
        //layoutView->Update();
        ContextMenu::GetMainInstance()->Update();
    }

    /// TODO: fix crash when deleting these editor layouts
    if (window != nullptr)
    {
        delete window;
    }
    /*if (layoutView != nullptr)
    {
        delete layoutView;
    }*/

    resources.FreeAll();

    TerminateOssium();
    return 0;
}
