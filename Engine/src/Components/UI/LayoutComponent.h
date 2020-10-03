#ifndef LAYOUTCOMPONENT_H
#define LAYOUTCOMPONENT_H

#include "../../Core/component.h"

namespace Ossium
{

    class LayoutSurface;

    class LayoutComponent : public Component
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component, LayoutComponent);
        
        // When called, the implementation should refresh the layout.
        virtual void LayoutRefresh() = 0;

    private:
        // Reference to the LayoutSurface.
        LayoutSurface* layoutSurface = nullptr;

    };

}

#endif // LAYOUTCOMPONENT_H
