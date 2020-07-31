#include "ToolBar.h"
#include "SceneHierarchy.h"
#include "../Examples/font_viewer.h"
#include "../Core/contextmenu.h"
#include "../Examples/demo_window_docking.h"
#include "../Examples/layout_diagram.h"

namespace Ossium::Editor
{

    void ToolBar::OnInit()
    {
        bordered = false;
        backgroundColor = Color(250, 250, 250);
        minDimensions.y = 16;

        EditorController* editor = GetEditorLayout()->GetEditorController();

        editor->AddCustomMenu("File/Add Window/Scene Hierarchy", [&] () { GetEditorLayout()->Add<SceneHierarchy>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("File/Add Window/Docking Demo", [&] () { GetEditorLayout()->Add<DemoDockingWindow>(this, DockingMode::BOTTOM); });
        editor->AddCustomMenu("File/Quit", [&] () { doQuit = true; });
        editor->AddCustomMenu("Edit/Undo", [] () {});
        editor->AddToolWindow<FontViewer>("View/Fonts");
        editor->AddCustomMenu("View/Layout", [&] () {
            EditorLayout* layout = GetEditorLayout()->GetEditorController()->AddLayout<LayoutDiagram>();
            // This is safe because we know there's only one window in the layout at this point.
            ((LayoutDiagram*)layout->GetLayout()->GetRoots()[0]->data.window)->target = GetEditorLayout();
        });

    }

    ToolBar::FuncPath::FuncPath(string id, vector<string> split, function<void()> func)
    {
        this->id = id;
        this->split = split;
        this->func = func;
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
                        found = toolTree.Insert(FuncPath(id, path, tool.onClick), previous);
                    }
                    else
                    {
                        found = toolTree.Insert(FuncPath(id, {}, {}), previous);
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
                                expansions.top()->Add(n->data.id, n->data.func);
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
