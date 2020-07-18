#include "ToolBar.h"
#include "../Examples/font_viewer.h"
#include "../Core/contextmenu.h"

namespace Ossium::Editor
{

    void ToolBar::OnInit()
    {
        bordered = false;
        EditorController* editor = GetEditorLayout()->GetEditorController();

        editor->AddCustomMenu("File/Quit", [&] () { doQuit = true; });
        editor->AddCustomMenu("Edit", [&] () {});
        editor->AddToolWindow<FontViewer>("View/Fonts");

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

        // TODO: make this a vector of pairs (to maintain order).
        map<string, vector<FuncPath>> menus;
        for (auto itr : editor->customMenuTools)
        {
            vector<string> path = Utilities::Split(itr.first, '/');
            if (path.size() < 2)
            {
                Log.Warning("Cannot add custom menu button at path '{0}' because it does not specify a dropdown option.", itr.first);
                continue;
            }
            string menuName = path[0];
            path.erase(path.begin());
            menus[menuName].push_back(FuncPath(itr.first, path, itr.second));
        }

        Vector2 oldPos;
        for (auto itr : menus)
        {
            oldPos = GetLayoutPosition();
            if (Button(itr.first))
            {
                vector<ContextMenu*> expansions = { &menu };
                vector<ContextMenu*> nextExpansions;
                unsigned int completed = 0;

                // Breadth-first traversal from the root menu
                for (auto expansion : expansions)
                {
                    ContextMenu& currentMenu = *expansion;
                    while (completed < itr.second.size())
                    {
                        completed = 0;
                        for (FuncPath& funcPath : itr.second)
                        {
                            if (funcPath.split.empty())
                            {
                                // Skip, already done
                                completed++;
                                continue;
                            }

                            string name = *funcPath.split.begin();
                            funcPath.split.erase(funcPath.split.begin());
                            if (funcPath.split.empty())
                            {
                                completed++;
                                currentMenu.Add(name, funcPath.func, nullptr, true);
                            }
                            else
                            {
                                nextExpansions.push_back(
                                    currentMenu.AddPopoutMenu(name, nullptr, true);
                                );
                            }
                        }
                    }
                    expansions = nextExpansions;
                    nextExpansions.clear();
                }
                menu.Show(Vector2(GetNativePosition().x + oldPos.x, GetCurrentBlockSize() + GetNativePosition().y));
            }
        }

        EndHorizontal();
    }

    bool ToolBar::ShouldQuit()
    {
        return doQuit;
    }

}
