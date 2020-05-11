#include "demo_window_docking.h"
#include "simple_demo_window.h"

namespace Ossium::Editor
{

    void DemoDockingWindow::OnGUI()
    {

        Space(20);
        TextLabel("<b><color=#ff0000>WINDOW DEMO</color></b>");
        Space(20);

        bool wasDocked = docked;
        docked = Button(docked ? "Undock Window" : "Dock Window") ? !docked : docked;
        if (docked != wasDocked)
        {
            if (docked)
            {
                sibling = GetNativeWindow()->Add<SimpleDemoWindow>(this, DockingMode::RIGHT);
            }
            else
            {
                sibling->Destroy();
                sibling = nullptr;
            }
        }

    }

}
