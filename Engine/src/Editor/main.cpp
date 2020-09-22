#include "../Ossium.h"
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

    resources.FreeAll();

    delete editor;
    editor = nullptr;

    TerminateOssium();
    return 0;
}
