#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "coremaths.h"
#include "ecs.h"
#include "globalservices.h"

namespace Ossium
{

    struct Transform2Schema : public Schema<Transform2Schema, 2>
    {
    public:
        DECLARE_BASE_SCHEMA(Transform2Schema, 2);

        /// Should this transform be tied to the parent transform?
        M(bool, tieToParent) = false;

    protected:
        /// Local/relative transform to parent.
        M(Transform, trans);

    };

    class OSSIUM_EDL Transform2 : public BaseComponent, public GlobalServices, public Transform2Schema
    {
    public:
        DECLARE_COMPONENT(Transform2);
        CONSTRUCT_SCHEMA(BaseComponent, Transform2Schema);

        /// Ref to the local transform position.
        Point& position();

        /// Ref to the local transform orientation.
        Rotation& rotation();

        /// Ref to the local transform itself.
        Transform& transform();

    };

}

#endif // TRANSFORM_H
