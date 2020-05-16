#include "demo_window_docking.h"
#include "simple_demo_window.h"

namespace Ossium::Editor
{

    void DemoDockingWindow::OnGUI()
    {

        Space(20);
        TextLabel("<b>WINDOW DEMO</b>");
        Space(20);

        bool wasDocked = docked;
        docked = Button(docked ? "Undock Window" : "Dock Window") ? !docked : docked;
        if (docked != wasDocked)
        {
            if (docked && sibling == nullptr)
            {
                sibling = GetNativeWindow()->Add<SimpleDemoWindow>(this, DockingMode::RIGHT);
            }
            else
            {
                sibling->Destroy();
                sibling = nullptr;
            }
        }

        if (Button("Print layout info"))
        {
            Logger::EngineLog().Info("Layout data:");
            for (auto node : GetNativeWindow()->GetLayout()->GetFlatTree())
            {
                Logger::EngineLog().Info("Node {0} (window = {1}, depth {2}), parent = {3},", node, node->data.window, node->depth, node->parent);
            }
        }

    }

}
