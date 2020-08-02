#include "SceneHierarchy.h"
#include "../Core/editorcontroller.h"
#include "../Core/editorstyle.h"

#include "../../Core/ecs.h"

using namespace Ossium;

namespace Ossium::Editor
{

    void SceneHierarchy::OnInit()
    {
        padding = 1;
    }

    void SceneHierarchy::OnGUI()
    {
        if (GetEditorLayout()->GetEditorController()->GetProject() != nullptr)
        {
            for (auto& scene : GetEditorLayout()->GetEditorController()->GetProject()->openScenes)
            {
                if (!IsVisible())
                {
                    // early out
                    break;
                }
                Scene* loaded = resources->Find<Scene>(scene.name);
                if (!loaded)
                {
                    // Show the name of the scene, but indicate it is not loaded.
                    scene.opened = false;
                    ListScene(scene, false);
                }
                else
                {
                    ListScene(scene, true);
                    loaded->WalkEntities([&] (Entity* entity) {
                        ListEntity(entity);
                    });
                }
            }
        }
    }

    void SceneHierarchy::ListScene(ListedScene& item, bool loaded)
    {
        if (IsVisible())
        {
            BeginHorizontal();

            float arrowSize = 9.0f;
            Vector2 arrowPos = GetLayoutPosition() + Vector2(4, 4);

            bool toggled = Toggle(
                loaded && item.opened,
                item.path == selectedScene ? EditorStyle::HierarchySceneSelected : EditorStyle::HierarchyScene,
                Vector2(8 + arrowSize, 8 + arrowSize),
                [&] (bool on) {
                    renderer->SetDrawColor(Colors::BLACK);
                    // Draw an arrow
                    Line arrowLine = Line(arrowPos, arrowPos + (on ? Vector2(arrowSize, 0) : Vector2(0, arrowSize)));

                    for (unsigned int i = 0, counti = (int)round(arrowSize / 2.0f); i < counti; i++)
                    {
                        arrowLine.Draw(*renderer);
                        arrowLine.a += Vector2::OneOne;
                        arrowLine.b += on ? Vector2(-1, 1) : Vector2::OneNegOne;
                    }
                }
            );
            if (loaded)
            {
                item.opened = toggled;
            }

            bool hovered, pressed;
            Button(item.name, item.path == selectedScene ? EditorStyle::HierarchySceneSelected : EditorStyle::HierarchyScene, false, 2, 2, true, &hovered, &pressed);
            if (hovered && pressed && !InputState.mouseWasPressed)
            {
                // On press down, select this scene
                selectedScene = item.path;
                TriggerUpdate();
            }

            EndHorizontal();

        }
    }

    void SceneHierarchy::ListEntity(Entity* entity)
    {
    }

}
