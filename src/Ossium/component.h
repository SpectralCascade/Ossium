#ifndef COMPONENT_H
#define COMPONENT_H

#include "transform.h"

namespace Ossium
{

    #define DECLARE_ABSTRACT_GRAPHIC_COMPONENT(TYPE)                    \
        DECLARE_ABSTRACT_COMPONENT(TYPE)

    #define REGISTER_ABSTRACT_GRAPHIC_COMPONENT(TYPE)                   \
        TYPE::TYPE() {}                                                 \
        TYPE::~TYPE() {}                                                \
        void TYPE::OnCreate() {}                                        \
        void TYPE::OnDestroy() {}                                       \
        void TYPE::OnClone() {}                                         \
        void TYPE::Update(){}                                           \
        void TYPE::OnInitGraphics(Renderer* renderer, int layer)        \
        {                                                               \
            GraphicComponent::OnInitGraphics(renderer, layer);          \
        }                                                               \
        void TYPE::OnRemoveGraphics()                                   \
        {                                                               \
            GraphicComponent::OnRemoveGraphics();                       \
        }

    class Component : public BaseComponent, public GlobalServices
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component);

        Point& WorldPosition();

        Rotation& WorldRotation();

        Transform& WorldTransform();

    };

    class GraphicComponent : public Graphic, public Component
    {
    public:
        /// Attempts to set the rendering layer of this graphic component. Note that you probably shouldn't call this
        /// too frequently as it attempts a removal from one set and insertion into another set within the renderer.
        void SetRenderLayer(int layer);

        /// Returns the layer this component is being rendered on.
        int GetRenderLayer();

    protected:
        DECLARE_ABSTRACT_COMPONENT(GraphicComponent);

        virtual void Render(Renderer& renderer) = 0;

        /// Pointer to the renderer instance this graphic component is registered to.
        Renderer* rendererInstance = nullptr;

    private:
        /// The rendering layer this graphic component should use.
        int renderLayer = 0;

    };

}

#endif // COMPONENT_H
