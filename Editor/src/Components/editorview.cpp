#include "editorview.h"

namespace Ossium::Editor
{

    void EditorWindow::OnGUI()
    {
        Begin();

        TextLabel("Testing...");
        TextLabel("----------");
        TextLabel("I hope this works... haha ha hahahah hah ah aa ha ha hah ha h hah ha ha ahha ha hhahahahahaha ah ah ha hahahhaha ahah hahaha haha");
        TextLabel(":D");

        TextField("Testing input...");
    }

}
