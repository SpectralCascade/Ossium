#include "SceneHierarchy.h"
#include "../Core/editorcontroller.h"
#include "../Core/editorstyle.h"

#include "../../Core/ecs.h"
#include "../Core/contextmenu.h"

using namespace Ossium;

namespace Ossium::Editor
{

    void SceneHierarchy::OnInit()
    {
        padding = 1;
    }

    void SceneHierarchy::OnGUI()
    {
        rightPressed = input->GetHandler<MouseHandler>()->RightPressed();
        Project* project = GetEditorLayout()->GetEditorController()->GetProject();
        if (project != nullptr)
        {
            for (auto& scene : project->openScenes)
            {
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
                            return true;
                        }, false);
                    }
                }
            }
        }
        rightWasPressed = rightPressed;
    }

    void SceneHierarchy::ListScene(ListedScene& item, bool loaded)
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
        if (hovered && ((pressed && !InputState.mouseWasPressed) || (rightPressed && !rightWasPressed)))
        {
            if (item.loaded)
            {
                // On press down, select this scene.
                GetEditorLayout()->GetEditorController()->SelectScene(resources->Find<Scene>(item.path));
                TriggerUpdate();
            }
            if (rightPressed && !rightWasPressed)
            {
                ContextMenu* cmenu = ContextMenu::GetMainInstance(resources);
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                cmenu->ClearOptions();

                cmenu->Add(item.loaded ? "Unload Scene" : "Load Scene", [&, project] () {
                    if (item.loaded)
                    {
                        resources->Free<Scene>(item.path);
                        item.loaded = false;
                        GetEditorLayout()->GetEditorController()->SelectScene(nullptr);
                        GetEditorLayout()->GetEditorController()->SelectEntity(nullptr);
                    }
                    else
                    {
                        item.loaded = resources->Get<Scene>(item.path, GetEditorLayout()->GetEditorController()->GetMainLayout()->GetServices()) != nullptr;
                    }
                });

                cmenu->Add("Remove Scene", [&, project] () {
                    if (item.loaded)
                    {
                        resources->Free<Scene>(item.path);
                        item.loaded = false;
                    }
                    // Remove from the open project scenes
                    for (auto itr = project->openScenes.begin(); itr != project->openScenes.end(); itr++)
                    {
                        if (&(*itr) == &item)
                        {
                            project->openScenes.erase(itr);
                            break;
                        }
                    }
                    GetEditorLayout()->GetEditorController()->SelectScene(nullptr);
                    GetEditorLayout()->GetEditorController()->SelectEntity(nullptr);
                });

                Rect vport = renderer->GetViewportRect();
                cmenu->Show(InputState.mousePos + renderer->GetWindow()->GetPosition() + Vector2(vport.x, vport.y));
            }
        }

        EndHorizontal();

    }

    void SceneHierarchy::ListEntity(Entity* entity)
    {
        BeginHorizontal();

        EditorController* editor = GetEditorLayout()->GetEditorController();

        entity->SetActive(Toggle(entity->IsActiveLocally()));

        StyleClickable style = editor->GetSelectedEntity() == entity ? EditorStyle::HierarchyEntitySelected : EditorStyle::HierarchyEntity;

        std::string text = "";
        for (int i = entity->GetDepth(); i >= 0; i--)
        {
            text += "  ";
        }
        text += entity->name;

        TextLayout tlayout;
        Vector2 layoutPos = GetLayoutPosition();
        Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x - 6, renderer->GetHeight() - 2);
        tlayout.mainColor = style.normalStyleText.fg;
        tlayout.mainStyle = style.normalStyleText.style;
        tlayout.SetPointSize(12);
        tlayout.SetBounds(limits);
        tlayout.SetAlignment(Typographic::TextAlignment::LEFT_ALIGNED);
        tlayout.SetText(*renderer, *editor->GetFont(), text, true);
        tlayout.Update(*editor->GetFont());

        bool hovered, pressed;
        Button(text, tlayout, style, false, 4, 2, true, &hovered, &pressed);
        if (hovered && ((pressed && !InputState.mouseWasPressed) || (rightPressed && !rightWasPressed)))
        {
            // On press down, select this scene.
            editor->SelectEntity(entity);
            editor->SelectScene(entity->GetScene());
            TriggerUpdate();

            if (rightPressed && !rightWasPressed)
            {
                ContextMenu* cmenu = ContextMenu::GetMainInstance(resources);
                cmenu->ClearOptions();

                cmenu->Add("Clone Entity", [&, entity] () {
                    GetEditorLayout()->GetEditorController()->SelectEntity(entity->Clone());
                });

                cmenu->Add("Delete Entity", [&, entity] () {
                    entity->Destroy(true);
                    GetEditorLayout()->GetEditorController()->SelectEntity(nullptr);
                });

                Rect vport = renderer->GetViewportRect();
                cmenu->Show(InputState.mousePos + renderer->GetWindow()->GetPosition() + Vector2(vport.x, vport.y));
            }
        }

        EndHorizontal();
    }

}
