#ifndef LAYOUTSURFACE_H
#define LAYOUTSURFACE_H

#include "../../Core/component.h"

namespace Ossium
{

    struct LayoutSurfaceSchema : public Schema<LayoutSurfaceSchema, 20>
    {
        DECLARE_BASE_SCHEMA(LayoutSurfaceSchema, 20);
        
    };


    class LayoutSurface : public Component, public LayoutSurfaceSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, LayoutSurfaceSchema);
        DECLARE_COMPONENT(Component, LayoutSurface);
        
        void OnLoadFinish();

        // Calls LayoutRefresh() for all active and enabled LayoutComponents beneath this LayoutSurface
        // (breadth-first order). Ignores LayoutComponents on and below entities with LayoutSurfaces which
        // are not marked dirty.
        void LayoutUpdate();

        // Is the layout dirty?
        bool IsDirty();

        // Mark the layout dirty.
        void SetDirty();

        // Cleans up LayoutComponents on and below this layout's entity, if any. 
        void OnDestroy();

    private:
        // Is the layout dirty?
        bool dirty = false;
        
    };

}

#endif // LAYOUTSURFACE_H
