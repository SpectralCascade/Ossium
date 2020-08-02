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
            didSelectScene = false;
            for (auto& scene : GetEditorLayout()->GetEditorController()->GetProject()->openScenes)
            {
                if (!IsVisible())
                {
                    // early out
                    break;
                }
                Scene* loaded = resources->Find<Scene>(scene.path);
                if (!loaded)
                {
                    // Show the name of the scene, but indicate it is not loaded.
                    scene.opened = false;
                    scene.loaded = false;
                    ListScene(scene, false);
                }
                else
                {
                    scene.loaded = true;
                    ListScene(scene, true);
                    if (scene.opened)
                    {
                        loaded->WalkEntities([&] (Entity* entity) {
                            ListEntity(entity);
                        });
                    }
                }
            }

            if (!didSelectScene && InputState.mouseWasPressed && !InputState.mousePressed)
            {
                // Must have clicked somewhere else in the window.
                GetEditorLayout()->GetEditorController()->SelectScene(nullptr);
            }
            if (!didSelectEntity && !didSelectScene && InputState.mouseWasPressed && !InputState.mousePressed)
            {
                // Must have clicked somewhere else in the window.
                GetEditorLayout()->GetEditorController()->SelectEntity(nullptr);
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

            Scene* selectedScene = GetEditorLayout()->GetEditorController()->GetSelectedScene();

            bool toggled = Toggle(
                loaded && item.opened,
                selectedScene != nullptr && item.name == selectedScene->GetName() ? EditorStyle::HierarchySceneSelected : EditorStyle::HierarchyScene,
                Vector2(8 + arrowSize, 8 + arrowSize),
                [&] (bool on) {
                    renderer->SetDrawColor(Colors::BLACK);
                    // Draw an arrow
                    Line arrowLine = Line(arrowPos, arrowPos + (on && loaded ? Vector2(arrowSize, 0) : Vector2(0, arrowSize)));

                    for (unsigned int i = 0, counti = (int)round(arrowSize / 2.0f); i < counti; i++)
                    {
                        arrowLine.Draw(*renderer);
                        arrowLine.a += Vector2::OneOne;
                        arrowLine.b += on && loaded ? Vector2(-1, 1) : Vector2::OneNegOne;
                    }
                }
            );
            if (loaded)
            {
                item.opened = toggled;
            }

            bool hovered, pressed;
            Button(item.name, selectedScene != nullptr && item.name == selectedScene->GetName() ? EditorStyle::HierarchySceneSelected : EditorStyle::HierarchyScene, false, 2, 2, true, &hovered, &pressed);
            if (hovered && pressed && !InputState.mouseWasPressed && item.loaded)
            {
                // On press down, select this scene.
                GetEditorLayout()->GetEditorController()->SelectScene(resources->Find<Scene>(item.path));
                didSelectScene = true;
                TriggerUpdate();
            }

            EndHorizontal();

        }
    }

    void SceneHierarchy::ListEntity(Entity* entity)
    {
        if (IsVisible())
        {
            BeginHorizontal();

            bool hovered, pressed;
            Button(entity->name, false ? EditorStyle::HierarchySceneSelected : EditorStyle::HierarchyScene, false, 2, 2, true, &hovered, &pressed);
            if (hovered && pressed && !InputState.mouseWasPressed)
            {
                // On press down, select this scene.
                GetEditorLayout()->GetEditorController()->SelectEntity(entity);
                didSelectEntity = true;
                TriggerUpdate();
            }

            EndHorizontal();
        }
    }

}
