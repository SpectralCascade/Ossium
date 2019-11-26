#include "transform.h"

namespace Ossium
{

    REGISTER_COMPONENT(Transform2);

    Point& Transform2::position()
    {
        return trans.position();
    }

    Rotation& Transform2::rotation()
    {
        return trans.rotation();
    }

    Transform& Transform2::transform()
    {
        return trans;
    }

}
