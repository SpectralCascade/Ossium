#include "rigidcircle.h"

namespace Ossium
{

    REGISTER_COMPONENT(CircleCollider);

    void CircleCollider::SetupShape()
    {
        shape.m_radius = PTM(radius);
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
        Ossium::Circle c = {
            .x = GetTransform()->GetWorldPosition().x,
            .y = GetTransform()->GetWorldPosition().y,
            .r = radius
        };
        c.Draw(renderer, Ossium::Colors::GREEN);
    }

}
