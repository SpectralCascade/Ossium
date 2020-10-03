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
        Rect(0, 0, viewport.w, viewport.h).DrawFilled(*renderer, Colors::WHITE);

        EditorController* editor = GetEditorLayout()->GetEditorController();

        vector<Scene*> loadedScenes = editor->GetLoadedScenes();
        for (Scene* scene : loadedScenes)
        {
            //scene->UpdateComponents();
            // Hacky fix for updating transform positions. Rather slow, alternative maybe?
            scene->WalkComponents<Transform>([] (Transform* t) { t->SetDirty(); });
            
            // Update all layouts
            scene->WalkEntities([] (Entity* entity) {
                if (entity->IsActive())
                {
                    LayoutSurface* layoutSurface = entity->GetComponent<LayoutSurface>();
                    if (layoutSurface && layoutSurface->enabled)
                    {
                        layoutSurface->LayoutUpdate();
                    }
                    return true;
                }
                return false;
            });
        }



        // Draw to viewport
        renderer->RenderPresent(true);

        for (Scene* scene : editor->GetLoadedScenes())
        {
            scene->DestroyPending();
        }

    }

}
