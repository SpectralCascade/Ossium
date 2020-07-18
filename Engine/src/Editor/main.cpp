#include "../Ossium.h"
#include "Examples/simple_demo_window.h"
#include "Examples/demo_window_docking.h"
#include "Examples/layout_diagram.h"
#include "Core/editorcontroller.h"

using namespace Ossium;
using namespace Ossium::Editor;
using namespace std;

int main(int argc, char* argv[])
{
    InitialiseOssium();

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    ResourceController resources;
    EditorController* editor = new EditorController(&resources);

    while (editor->Update())
    {
        // Update until the editor stops running.
    }

    delete editor;
    editor = nullptr;

    resources.FreeAll();

    TerminateOssium();
    return 0;
}
