#include "editorview.h"

namespace Ossium::Editor
{

    void EditorWindow::OnGUI()
    {
        Begin();

        TextLabel("Testing...");
        TextLabel("----------");
        TextLabel("I hope this works...");
        TextLabel(":D");
    }

}
