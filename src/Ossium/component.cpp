#include "component.h"

namespace Ossium
{

    REGISTER_ABSTRACT_COMPONENT(Component);

    REGISTER_ABSTRACT_GRAPHIC_COMPONENT(GraphicComponent);

    ResourceController GlobalServices::Resources;

    Renderer* GlobalServices::MainRenderer = nullptr;

}
