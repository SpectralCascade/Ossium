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
            FuncPath(std::string id, std::vector<std::string> split, std::function<void()> func, std::function<bool()> isEnabled);
            FuncPath() = default;
            std::string id;
            std::vector<std::string> split;
            std::function<void()> func;
            std::function<bool()> isEnabled;
        };

    };

}

#endif // TOOLBAR_H
