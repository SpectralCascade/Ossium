#include "boxcollider.h"

namespace Ossium
{

    REGISTER_COMPONENT(BoxCollider);

    void BoxCollider::SetupShape()
    {
        shape.SetAsBox(PTM(width / 2.0f), PTM(height / 2.0f));
    }

    const b2Shape& BoxCollider::GetShape()
    {
        return shape;
    }

    const b2PolygonShape& BoxCollider::GetPolygonShape()
    {
        return shape;
    }

    void BoxCollider::Render(Renderer& renderer)
    {
        Ossium::Rect box = {
            .x = GetTransform()->GetWorldPosition().x - (width / 2),
            .y = GetTransform()->GetWorldPosition().y - (height / 2),
            .w = width,
            .h = height
        };
        box.Draw(renderer, Ossium::Colors::GREEN);
    }

/* TODO: rebuild physics body
    void BoxCollider::SetWidth(float width)
    {
        this->width = width;
        shape.SetAsBox(PTM(width / 2.0f), PTM(height / 2.0f));
    }
    void BoxCollider::SetHeight(float height)
    {
        this->height = height;
        shape.SetAsBox(PTM(width / 2.0f), PTM(height / 2.0f));
    }

    float BoxCollider::GetWidth()
    {
        return width;
    }
    float BoxCollider::GetHeight()
    {
        return height;
    }
*/
}
