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

        unsigned int selected = 0;

        struct FuncPath
        {
            FuncPath(string id, vector<string> split, function<void()> func);
            FuncPath() = default;
            string id;
            vector<string> split;
            function<void()> func;
        };

    };

}

#endif // TOOLBAR_H
