#include "rigidcircle.h"

namespace Ossium
{

    REGISTER_COMPONENT(CircleCollider);

    void CircleCollider::SetupShape()
    {
        shape.m_radius = PTM(radius);
        shape.m_p = offset;
    }

    const b2Shape& CircleCollider::GetShape()
    {
        return shape;
    }

    const b2CircleShape& CircleCollider::GetCircleShape()
    {
        return shape;
    }

    void CircleCollider::Render(Renderer& renderer)
    {
#ifdef OSSIUM_EDITOR
        Ossium::Circle c = {
            .x = GetTransform()->GetWorldPosition().x + offset.x,
            .y = GetTransform()->GetWorldPosition().y + offset.y,
            .r = radius
        };
        c.Draw(renderer, Ossium::Colors::GREEN);
#endif // OSSIUM_EDITOR
    }

}
