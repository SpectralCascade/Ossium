#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "../../OssiumEditor.h"

namespace Ossium::Editor
{

    class ToolBar : public EditorWindow, public Service<ToolBar>
    {
    public:
        bool ShouldQuit();

    protected:
        void OnGUI();

        void OnInit();

    private:
        bool doQuit = false;

        struct FuncPath
        {
            FuncPath(string id, vector<string> split, function<void()> func);
            string id;
            vector<string> split;
            function<void()> func;
        };

        struct MenuData
        {
            string name;
            vector<FuncPath> path;
        };

    };

}

#endif // TOOLBAR_H
