#include "SceneView.h"
#include "../Core/editorcontroller.h"

using namespace std;

namespace Ossium::Editor
{

    void SceneView::OnInit()
    {
        alwaysUpdate = true;
    }

    void SceneView::OnGUI()
    {
        EditorController* editor = GetEditorLayout()->GetEditorController();

        vector<Scene*> loadedScenes = editor->GetLoadedScenes();
        for (Scene* scene : loadedScenes)
        {
            scene->UpdateComponents();
        }

        // Draw to viewport
        renderer->RenderPresent(true);

        for (Scene* scene : loadedScenes)
        {
            scene->DestroyPending();
        }

    }

}
