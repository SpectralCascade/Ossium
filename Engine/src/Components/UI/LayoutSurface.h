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
        
    };

}

#endif // LAYOUTSURFACE_H
