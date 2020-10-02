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

        // Marks this layout component for a layout refresh.
        void LayoutSetDirty();

        // Returns true if this layout is marked dirty.
        bool LayoutIsDirty();

        // Calls LayoutRefresh() then recursively calls for all DIRECT children.
        // This means that if this entity has a child that does not have a BoxLayout,
        // but that child entity has BoxLayouts in it's children, those BoxLayouts will NOT be updated!
        void LayoutUpdate();

    private:
        // Indicates whether this layout component needs to be refreshed or not.
        bool layoutDirty = true;

        // Reference to the LayoutSurface.
        LayoutSurface* layoutSurface = nullptr;

    };

}

#endif // LAYOUTCOMPONENT_H
