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

        BeginHorizontal();
        TextField("Testing input...");
        Space(30);
        TextField("000");
        TextField("100%");
        EndHorizontal();

        Space(50);

        BeginHorizontal();
        TextLabel("Checkbox: ");
        bool checked = Toggle(false);
        EndHorizontal();

        Space(50);

        if (Button("cool button", 40))
        {
            Logger::EngineLog().Info("Neuron button clicked!");
        }
        Space(10);
        if (Button("wow", 100, 20))
        {
            Logger::EngineLog().Info("Wowzas");
        }

    }

}
