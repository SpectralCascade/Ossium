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
        editor->AddCustomMenu("Edit/Undo", [&] () {});
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

        unordered_map<string, unsigned int> menuNameMap;
        vector<MenuData> menus;
        for (auto tool : editor->customMenuTools)
        {
            vector<string> path = Utilities::Split(tool.path, '/');
            if (path.size() < 2)
            {
                Log.Warning("Cannot add custom menu button at path '{0}' because it does not specify a dropdown option.", tool.path);
                continue;
            }
            string menuName = path[0];
            path.erase(path.begin());

            auto pos = menuNameMap.find(menuName);
            if (pos == menuNameMap.end())
            {
                pos = menuNameMap.emplace(menuName, menus.size()).first;
                menus.push_back(MenuData());
            }
            menus[pos->second].name = menuName;
            menus[pos->second].path.push_back(FuncPath(tool.path, path, tool.onClick));
        }

        Vector2 oldPos;
        for (auto menuRoot : menus)
        {
            oldPos = GetLayoutPosition();
            if (Button(menuRoot.name))
            {
                // Always clear previous options
                menu.ClearOptions();

                vector<ContextMenu*> expansions = { &menu };
                vector<ContextMenu*> nextExpansions;
                unsigned int completed = 0;

                // Breadth-first traversal from the root menu
                for (auto expansion : expansions)
                {
                    ContextMenu& currentMenu = *expansion;
                    while (completed < menuRoot.path.size())
                    {
                        completed = 0;
                        for (FuncPath& funcPath : menuRoot.path)
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
                                    currentMenu.AddPopoutMenu(name, nullptr, true)
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
