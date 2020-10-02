#include "LayoutSurface.h"
#include "LayoutComponent.h"

namespace Ossium
{

    REGISTER_COMPONENT(LayoutSurface);

    void LayoutSurface::OnLoadFinish()
    {
        // Update layout components
        LayoutComponent* local = entity->GetComponent<LayoutComponent>();
        if (local)
        {
            local->LayoutUpdate();
        }
        else
        {
            for (auto layout : entity->GetComponentsInChildren<LayoutComponent>())
            {
                layout->LayoutUpdate();
            }
        }
    }

}
