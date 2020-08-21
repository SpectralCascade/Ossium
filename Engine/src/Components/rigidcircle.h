#ifndef RIGIDCIRCLE_H
#define RIGIDCIRCLE_H

#include "collider.h"

namespace Ossium
{

    struct CircleColliderSchema : public Schema<CircleColliderSchema, 1>
    {
        DECLARE_BASE_SCHEMA(CircleColliderSchema, 1);

        M(float, radius) = 1.0f;

    };

    class CircleCollider : public Collider, public CircleColliderSchema
    {
    public:
        CONSTRUCT_SCHEMA(Collider, CircleColliderSchema);
        DECLARE_COMPONENT(Collider, CircleCollider);

        /// Return an immutable reference to the shape.
        const b2Shape& GetShape();

        /// Return the shape as it's true type.
        const b2CircleShape& GetCircleShape();

        // Used for debugging or in editor.
        void Render(Renderer& renderer);

    private:
        b2CircleShape shape;

    };

}

#endif // RIGIDCIRCLE_H
