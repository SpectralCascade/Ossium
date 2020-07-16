#include "ToolBar.h"
#include "../Core/contextmenu.h"

namespace Ossium::Editor
{

    void ToolBar::OnInit()
    {
        bordered = false;
    }

    void ToolBar::OnGUI()
    {
        BeginHorizontal();

        if (Button("File"))
        {
            ContextMenu& menu = *ContextMenu::GetMainInstance(resources);
            menu.SetOptions({
                ContextMenu::Option("Quit", [&] () { doQuit = true; })
            });
            menu.Show(Vector2(GetNativePosition().x, GetCurrentBlockSize() + GetNativePosition().y));
        }
        if (Button("Edit"))
        {
        }
        if (Button("View"))
        {
        }

        EndHorizontal();
    }

    bool ToolBar::ShouldQuit()
    {
        return doQuit;
    }

}
