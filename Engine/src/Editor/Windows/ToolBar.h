#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "../Core/editorwindow.h"

namespace Ossium::Editor
{

    class ToolBar : public EditorWindow, public Service<ToolBar>
    {
    public:
        void AddMenu(string name);

        bool ShouldQuit();

    protected:
        void OnGUI();

        void OnInit();

    private:
        bool doQuit = false;

    };

}

#endif // TOOLBAR_H
