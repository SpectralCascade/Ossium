#ifndef RIGIDCIRCLE_H
#define RIGIDCIRCLE_H

#include "collider.h"

namespace Ossium
{

    struct CircleColliderSchema : public Schema<CircleColliderSchema, 20>
    {
        DECLARE_BASE_SCHEMA(CircleColliderSchema, 20);

        M(float, radius) = 1.0f;

    };

    class CircleCollider : public Collider, public CircleColliderSchema
    {
    public:
        CONSTRUCT_SCHEMA(Collider, CircleColliderSchema);
        DECLARE_COMPONENT(Collider, CircleCollider);

        /// Setup the shape.
        void SetupShape();

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
