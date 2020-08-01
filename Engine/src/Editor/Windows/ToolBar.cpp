#include "ToolBar.h"
#include "SceneHierarchy.h"
#include "../Examples/font_viewer.h"
#include "../Core/contextmenu.h"
#include "../Examples/demo_window_docking.h"
#include "../Examples/layout_diagram.h"
#include "../../Core/engineconstants.h"
#include "../Core/editorconstants.h"
#include "../Core/tinyfiledialogs.h"

using namespace std;

namespace Ossium::Editor
{

    void ToolBar::OnInit()
    {
        bordered = false;
        backgroundColor = Color(250, 250, 250);
        minDimensions.y = 16;

        EditorController* editor = GetEditorLayout()->GetEditorController();

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
        editor->AddCustomMenu("File/Open...", [&] () {
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
                }

            }
        );

        editor->AddCustomMenu("File/Save", [&] () {
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                if (project != nullptr)
                {
                    project->Save(project->GetPath());
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

        // TODO: edit menus
        //editor->AddCustomMenu("Edit/Undo", [] () {});
        //editor->AddCustomMenu("Edit/Redo", [] () {});

        editor->AddCustomMenu("Create/Scene",
            [&] () {
                Project* project = GetEditorLayout()->GetEditorController()->GetProject();
                if (project != nullptr)
                {
                    Scene* newScene = new Scene();
                    string path = project->GetAssetsPath() + "Untitled" + EngineConstants::SceneFileExtension;
                    newScene->Save(path);
                    delete newScene;
                    resources->LoadAndInit<Scene>(path, GetEditorLayout()->GetEditorController()->GetMainLayout()->GetServices());
                }
            },
            [&] () { return GetEditorLayout()->GetEditorController()->GetProject() != nullptr; }
        );

        editor->AddCustomMenu("View/Add Window/Scene Hierarchy", [&] () { GetEditorLayout()->Add<SceneHierarchy>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("View/Add Window/Docking Demo", [&] () { GetEditorLayout()->Add<DemoDockingWindow>(this, DockingMode::BOTTOM); });
        editor->AddToolWindow<FontViewer>("View/Fonts");
        editor->AddCustomMenu("View/Layout", [&] () {
                EditorLayout* layout = GetEditorLayout()->GetEditorController()->AddLayout<LayoutDiagram>();
                // This is safe because we know there's only one window in the layout at this point.
                ((LayoutDiagram*)layout->GetLayout()->GetRoots()[0]->data.window)->target = GetEditorLayout();
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
            if (path.size() < 2)
            {
                Log.Warning("Cannot add custom menu button at path '{0}' because it does not specify a dropdown option.", tool.path);
                continue;
            }

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
            // TODO: once the context menu is open, you should be able to hover other buttons to show the appropriate menu without clicking.
            if (Button(menuRoot->data.id, EditorStyle::ToolBarButton, false, 6, 4))
            {
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
