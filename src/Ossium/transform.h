#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "coremaths.h"
#include "ecs.h"
#include "globalservices.h"

namespace Ossium
{

    struct Transform2Schema : public Schema<Transform2Schema, 1>
    {
    public:
        DECLARE_BASE_SCHEMA(Transform2Schema, 1);

    protected:
        M(Transform, trans);

    };

    class Transform2 : public BaseComponent, public GlobalServices, public Transform2Schema
    {
    public:
        DECLARE_COMPONENT(Transform2);
        CONSTRUCT_SCHEMA(BaseComponent, Transform2Schema);

        /// Get the transform position
        Point& position();

        /// Get the transform orientation
        Rotation& rotation();

        /// Get the actual transform
        Transform& transform();

    };

}

#endif // TRANSFORM_H
