#ifndef COMPONENT_H
#define COMPONENT_H

#include "ecs.h"
#include "resourcecontroller.h"

namespace Ossium
{

    /// Provides static access to various services, e.g. input controller, resources controller
    /// but only for types that inherit from it.
    class GlobalServices
    {
    protected:
        static ResourceController Resources;

    };

    class Component : public BaseComponent, public GlobalServices
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component);

    };

    class GraphicComponent : public BaseGraphicComponent, public GlobalServices
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(GraphicComponent);

    };

}

#endif // COMPONENT_H
