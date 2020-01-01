#include "editorview.h"

namespace Ossium::Editor
{

    void EditorWindow::OnGUI()
    {
        TextLabel("Testing...");
        TextLabel("----------");
        TextLabel("I hope this works... haha ha hahahah hah ah aa ha ha hah ha h hah ha ha ahha ha hhahahahahaha ah ah ha hahahhaha ahah hahaha haha");
        TextLabel(":D");

        BeginHorizontal();
        TextField("Testing input...");
        Tab();
        TextField("000");
        Tab();
        TextField("100%");
        EndHorizontal();

        Space(50);

        BeginHorizontal();
        TextLabel("Checkbox 1: ");
        Tab();
        Toggle(false);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Checkbox 2: ");
        Tab();
        Toggle(true);
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
