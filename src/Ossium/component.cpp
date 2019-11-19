#include "component.h"

namespace Ossium
{

    REGISTER_ABSTRACT_COMPONENT(Component, BaseComponent);

    ///
    /// Component
    ///

    Point& Component::WorldPosition()
    {
        return entity->AddComponentOnce<Transform2>()->position();
    }

    Rotation& Component::WorldRotation()
    {
        return entity->AddComponentOnce<Transform2>()->rotation();
    }

    Transform& Component::WorldTransform()
    {
        return entity->AddComponentOnce<Transform2>()->transform();
    }

    ///
    /// GraphicComponent
    ///

    void GraphicComponent::SetRenderLayer(int layer)
    {
        if (GetService<Renderer>() != nullptr)
        {
            if (renderLayer >= 0)
            {
                GetService<Renderer>()->Unregister(this, renderLayer);
            }
            renderLayer = GetService<Renderer>()->Register(this, layer);
        }
    }

    int GraphicComponent::GetRenderLayer()
    {
        return renderLayer;
    }

    GraphicComponent::GraphicComponent()
    {
    }

    GraphicComponent::~GraphicComponent()
    {
    }

    void GraphicComponent::OnCreate()
    {
        Component::OnCreate();
    }

    void GraphicComponent::OnLoadStart()
    {
        Component::OnLoadStart();
        if (GetService<Renderer>() != nullptr && renderLayer >= 0)
        {
            GetService<Renderer>()->Unregister(this, renderLayer);
            renderLayer = -1;
        }
    }

    void GraphicComponent::OnLoadFinish()
    {
        Component::OnLoadFinish();
        if (GetService<Renderer>() != nullptr)
        {
            renderLayer = GetService<Renderer>()->Register(this, renderLayer);
        }
    }

    void GraphicComponent::OnDestroy()
    {
        Component::OnDestroy();
        if (GetService<Renderer>() != nullptr && renderLayer >= 0)
        {
            GetService<Renderer>()->Unregister(this, renderLayer);
            renderLayer = -1;
        }
    }

    void GraphicComponent::OnClone(BaseComponent* src)
    {
    }

    void GraphicComponent::Update()
    {
    }

}
