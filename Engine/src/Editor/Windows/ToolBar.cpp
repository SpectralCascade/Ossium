#include "ToolBar.h"
#include "../Examples/font_viewer.h"
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

        ContextMenu& menu = *ContextMenu::GetMainInstance(resources);
        Vector2 oldPos = GetLayoutPosition();
        if (Button("File"))
        {
            menu.SetOptions({
                ContextMenu::Option("Quit", [&] () { doQuit = true; })
            });
            menu.Show(Vector2(GetNativePosition().x, GetCurrentBlockSize() + GetNativePosition().y));
        }
        oldPos = GetLayoutPosition();
        if (Button("Edit"))
        {
        }
        oldPos = GetLayoutPosition();
        if (Button("View"))
        {
            menu.SetOptions({
                ContextMenu::Option("Fonts", [&] () { GetEditorLayout()->GetEditorController()->AddLayout<FontViewer>(); })
            });
            menu.Show(Vector2(GetNativePosition().x + oldPos.x, GetCurrentBlockSize() + GetNativePosition().y));
        }

        EndHorizontal();
    }

    bool ToolBar::ShouldQuit()
    {
        return doQuit;
    }

}
