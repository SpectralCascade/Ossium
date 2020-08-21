#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "collider.h"

namespace Ossium
{

    struct BoxColliderSchema : public Schema<BoxColliderSchema, 2>
    {
    public:
        DECLARE_BASE_SCHEMA(BoxColliderSchema, 2);

    protected:
        M(float, width) = 64;
        M(float, height) = 64;

    };

    class BoxCollider : public Collider, public BoxColliderSchema
    {
    public:
        CONSTRUCT_SCHEMA(Collider, BoxColliderSchema);
        DECLARE_COMPONENT(Collider, BoxCollider);

        void OnLoadFinish();

        const b2Shape& GetShape();

        const b2PolygonShape& GetPolygonShape();

        void SetWidth(float width);
        void SetHeight(float height);

        float GetWidth();
        float GetHeight();

        void Render(Renderer& renderer);

    private:
        b2PolygonShape shape;

    };

}

#endif // BOXCOLLIDER_H
