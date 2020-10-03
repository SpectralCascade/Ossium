#include "ToolBar.h"
#include "SceneHierarchy.h"
#include "EntityProperties.h"
#include "SceneView.h"
#include "../Examples/font_viewer.h"
#include "../Core/contextmenu.h"
#include "../Examples/demo_window_docking.h"
#include "../Examples/layout_diagram.h"
#include "../../Core/engineconstants.h"
#include "../Core/editorconstants.h"
#include "../Core/tinyfiledialogs.h"
#include "../Core/project.h"
#include "../../Core/ecs.h"

#include <cstdlib>
#include <filesystem>

using namespace std;
using namespace Ossium;

namespace Ossium::Editor
{

    void ToolBar::OnInit()
    {
        bordered = false;
        backgroundColor = Color(250, 250, 250);
        minDimensions.y = 16;

        EditorController* editor = GetEditorLayout()->GetEditorController();

        //
        // File
        //

        editor->AddCustomMenu("File/New/Project", [&] () {
                // Close the context menu before pausing the editor
                ContextMenu::GetMainInstance(resources)->Hide();

                Project* project = GetEditorLayout()->GetEditorController()->CreateProject();
                project->SetName("Untitled");
                project->SetPath(EDITOR_DEFAULT_DIRECTORY);

                if (!project->SaveAs())
                {
                    GetEditorLayout()->GetEditorController()->CloseProject();
                }

            }
        );

        editor->AddCustomMenu("File/Open/Project", [&] () {
                ContextMenu::GetMainInstance(resources)->Hide();

                const char* filters[1] = { "*.ossium" };

                const char* path = tinyfd_openFileDialog(
                    "Ossium | Open Project",
                    EDITOR_DEFAULT_DIRECTORY,
                    1,
                    filters,
                    "Ossium Project",
                    0
                );

                if (path)
                {
                    GetEditorLayout()->GetEditorController()->OpenProject(path);

                    // TODO: load previous window layout instead of hardcoding this
                    SceneHierarchy* hierarchy = GetEditorLayout()->Add<SceneHierarchy>(this, DockingMode::BOTTOM);
                    SceneView* view = GetEditorLayout()->Add<SceneView>(hierarchy, DockingMode::RIGHT);
                    GetEditorLayout()->Add<EntityProperties>(view, DockingMode::RIGHT);
                    viewport.h = minDimensions.y;
                    GetEditorLayout()->Resize(this, Rect(viewport));
                }

            }
        );

        editor->AddCustomMenu("File/Open/Scene", [&] () {

                Project* project = GetEditorLayout()->GetEditorController()->GetProject();

                if (project != nullptr)
                {
                    ContextMenu::GetMainInstance(resources)->Hide();

                    string filter = string("*") + EngineConstants::SceneFileExtension;
                    const char* filters[1] = { filter.c_str() };

                    const char* path = tinyfd_openFileDialog(
                        "Ossium | Open Scene",
                        EDITOR_DEFAULT_DIRECTORY,
                        1,
                        filters,
                        "Ossium Scene",
                        0
                    );

                    if (path)
                    {
                        if (Utilities::Pick<ListedScene>(project->openScenes, [&] (ListedScene& scene) { return scene.path == string(path); }) == nullptr)
                        {
                            project->openScenes.push_back((ListedScene) {
                                .name = filesystem::path(string(path)).stem().string(),
                                .path = string(path),
                                .opened = true,
                                .loaded = resources->Get<Scene>(path, GetEditorLayout()->GetEditorController()->GetMainLayout()->GetServices()) != nullptr
                            });
                        }
                    }
                }
                else
                {
                    Log.Warning("Cannot open scene, no project is open!");
                }

            },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("File/Save", [&] () {
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                if (project != nullptr)
                {
                    if (project->GetPath().empty())
                    {
                        // In theory this shouldn't happen, but just in case!
                        project->SaveAs();
                    }
                    else
                    {
                        project->Save(project->GetPath());
                    }
                    if (!project->GetPath().empty())
                    {
                        for (auto& listedScene : project->openScenes)
                        {
                            if (listedScene.loaded)
                            {
                                Scene* scene = resources->Find<Scene>(listedScene.path);
                                if (scene != nullptr)
                                {
                                    scene->Save(listedScene.path);
                                }
                            }
                        }
                    }
                }
                else
                {
                    Log.Warning("Cannot save because no project is loaded.");
                }
            },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("File/Save as...", [&] () {
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                if (project != nullptr)
                {
                    ContextMenu::GetMainInstance(resources)->Hide();
                    project->SaveAs();
                }
                else
                {
                    Log.Warning("Cannot save because no project is loaded.");
                }
            },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("File/Close Project",
            [&] () { GetEditorLayout()->GetEditorController()->CloseProject(); },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("File/Quit", [&] () { doQuit = true; });

        //
        // Edit
        //

        // TODO: edit menus
        //editor->AddCustomMenu("Edit/Undo", [] () {});
        //editor->AddCustomMenu("Edit/Redo", [] () {});


        //
        // Create
        //

        editor->AddCustomMenu("Create/Scene",
            [&] () {
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                if (project != nullptr)
                {
                    Scene newScene = Scene(GetEditorLayout()->GetEditorController()->GetMainLayout()->GetServices());
                    string path = (filesystem::path(project->GetPath()) / (string("NewScene") + EngineConstants::SceneFileExtension)).string();

                    const char* filters[2] = { (string("*") + EngineConstants::SceneFileExtension).c_str(), "*" };

                    const char* dest = tinyfd_saveFileDialog(
                        "Ossium | Save Scene",
                        path.c_str(),
                        2,
                        filters,
                        "Ossium Scene (.rawr)"
                    );

                    if (dest)
                    {
                        newScene.SetName(filesystem::path(string(dest)).stem().string());
                        if (newScene.Save(dest))
                        {
                            project->openScenes.push_back((ListedScene){
                                .name = newScene.GetName(),
                                .path = string(dest),
                                .opened = true,
                                .loaded = resources->Get<Scene>(dest, GetEditorLayout()->GetEditorController()->GetMainLayout()->GetServices()) != nullptr
                            });
                        }
                        else
                        {
                            Log.Warning("Failed to save new scene at '{0}'!", dest);
                        }
                    }
                }
            },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("Create/Entity",
            [&] () {
                EditorController* e = GetEditorLayout()->GetEditorController();
                Project* project = e->GetProject();
                if (project != nullptr)
                {
                    Entity* selectedEntity = e->GetSelectedEntity();
                    if (selectedEntity != nullptr)
                    {
                        // Nest under this entity
                        selectedEntity->CreateChild();
                    }
                    else
                    {
                        Scene* selectedScene = e->GetSelectedScene();
                        if (selectedScene != nullptr)
                        {
                            // Add to end of the scene
                            selectedScene->CreateEntity();
                        }
                        else
                        {
                            // Find any loaded scene and use that.
                            for (auto& scene : project->openScenes)
                            {
                                if (scene.loaded)
                                {
                                    // Open the scene if not already
                                    scene.opened = true;
                                    Scene* insertScene = resources->Find<Scene>(scene.path);
                                    if (insertScene != nullptr)
                                    {
                                        insertScene->CreateEntity();
                                    }
                                    else
                                    {
                                        Log.Warning("Failed to create entity! No scenes are loaded.");
                                    }
                                }
                            }
                        }
                    }
                }
            },
            [&] () {
                Project* p = GetEditorLayout()->GetEditorController()->GetProject();
                return p != nullptr && Utilities::Pick<ListedScene>(p->openScenes, [] (ListedScene& scene) { return scene.loaded; }) != nullptr;
            }
        );

        //
        // View
        //

        /*editor->AddCustomMenu("View/Add Window/Scene Hierarchy", [&] () { GetEditorLayout()->Add<SceneHierarchy>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("View/Add Window/Entity Properties", [&] () { GetEditorLayout()->Add<EntityProperties>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("View/Add Window/Scene View", [&] () { GetEditorLayout()->Add<SceneView>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("View/Add Window/Docking Demo", [&] () { GetEditorLayout()->Add<DemoDockingWindow>(this, DockingMode::BOTTOM); });
        editor->AddToolWindow<FontViewer>("View/Fonts");
        editor->AddCustomMenu("View/Layout", [&] () {
                EditorLayout* layout = GetEditorLayout()->GetEditorController()->AddLayout<LayoutDiagram>();
                // This is safe because we know there's only one window in the layout at this point.
                ((LayoutDiagram*)layout->GetLayout()->GetRoots()[0]->data.window)->target = GetEditorLayout();
            }
        );*/

        editor->AddCustomMenu("Play in-game!", [&] () {
                string command = "ossium.exe ";
                for (auto scene : GetEditorLayout()->GetEditorController()->GetProject()->openScenes)
                {
                    if (scene.loaded)
                    {
                        command += Utilities::Format("\"{0}\" ", scene.path);
                    }
                }
                system(command.c_str());
            },
            [&] () {
                return GetEditorLayout()->GetEditorController()->GetProject() != nullptr && !GetEditorLayout()->GetEditorController()->GetProject()->openScenes.empty();
            }
        );

    }

    ToolBar::FuncPath::FuncPath(string id, vector<string> split, function<void()> func, function<bool()> isEnabled)
    {
        this->id = id;
        this->split = split;
        this->func = func;
        this->isEnabled = isEnabled;
    }

    void ToolBar::OnGUI()
    {
        BeginHorizontal();

        ContextMenu& menu = *ContextMenu::GetMainInstance(resources);
        EditorController* editor = GetEditorLayout()->GetEditorController();

        Tree<FuncPath> toolTree;
        for (auto tool : editor->customMenuTools)
        {
            vector<string> path = Utilities::Split(tool.path, '/');

            Node<FuncPath>* previous = nullptr;
            for (auto i = path.begin(); i != path.end(); i++)
            {
                string id = *i;
                Node<FuncPath>* found = previous != nullptr ?
                    Utilities::PickPointer<Node<FuncPath>*>(previous->children, [id] (auto n) { return n->data.id == id; })
                    : toolTree.Find([id] (auto n) { return n->data.id == id; });

                if (found == nullptr)
                {
                    if (i == path.end() - 1)
                    {
                        found = toolTree.Insert(FuncPath(id, path, tool.onClick, tool.isEnabled), previous);
                    }
                    else
                    {
                        found = toolTree.Insert(FuncPath(id, {}, {}, [] () { return true; }), previous);
                    }
                }
                previous = found;
            }
        }

        Vector2 oldPos;
        auto roots = toolTree.GetRoots();
        for (Node<FuncPath>* menuRoot : roots)
        {
            oldPos = GetLayoutPosition();
            if (menuRoot->children.empty())
            {
                // Regular button
                if (menuRoot->data.isEnabled() && Button(menuRoot->data.id, EditorStyle::StandardButton, true, 6, 4))
                {
                    // Call the function
                    menuRoot->data.func();
                }
            }
            else if (Button(menuRoot->data.id, EditorStyle::ToolBarButton, false, 6, 4))
            {
                // TODO: once the context menu is open, you should be able to hover other buttons to show the appropriate menu without clicking.
                // Always clear previous options
                menu.ClearOptions();

                stack<ContextMenu*> expansions;
                expansions.push(&menu);

                toolTree.Walk(
                    [&] (auto n) {
                        if (n != menuRoot && !n->children.empty())
                        {
                            // On the walk down, add expansion menus
                            expansions.push(expansions.top()->AddPopoutMenu(n->data.id));
                        }
                        return true;
                    },
                    [&] (auto n) {
                        if (!expansions.empty())
                        {
                            // On the walk back up, add options
                            if (!n->children.empty())
                            {
                                expansions.pop();
                            }
                            else if (!expansions.empty())
                            {
                                expansions.top()->Add(n->data.id, n->data.func, nullptr, n->data.isEnabled());
                            }
                        }
                        return true;
                    },
                    menuRoot
                );

                menu.Show(Vector2(GetNativePosition().x + oldPos.x, GetCurrentBlockSize() + GetNativePosition().y));
            }
        }

        EndHorizontal();

        minDimensions.y = GetLayoutPosition().y;

    }

    bool ToolBar::ShouldQuit()
    {
        return doQuit;
    }

}
