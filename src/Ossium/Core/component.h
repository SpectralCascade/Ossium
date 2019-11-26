#ifndef COMPONENT_H
#define COMPONENT_H

#include "transform.h"

namespace Ossium
{

    class Component : public BaseComponent, public GlobalServices
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component);

        Point& WorldPosition();

        Rotation& WorldRotation();

        Transform& WorldTransform();

    };

    struct GraphicComponentSchema : public Schema<GraphicComponentSchema, 1>
    {
    public:
        DECLARE_BASE_SCHEMA(GraphicComponentSchema, 1);

    protected:
        M(int, renderLayer) = -1;

    };

    class GraphicComponent : public Graphic, public Component, public GraphicComponentSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, GraphicComponentSchema);

        /// Attempts to set the rendering layer of this graphic component. Note that you probably shouldn't call this
        /// too frequently as it attempts a removal from one set and insertion into another set within the renderer.
        void SetRenderLayer(int layer);

        /// Returns the layer this component is being rendered on.
        int GetRenderLayer();

    protected:
        DECLARE_ABSTRACT_COMPONENT(GraphicComponent);

        virtual void Render(Renderer& renderer) = 0;

    private:
        using GraphicComponentSchema::renderLayer;

    };

}

#endif // COMPONENT_H
