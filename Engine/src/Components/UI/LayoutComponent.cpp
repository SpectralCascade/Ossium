#include "LayoutComponent.h"
#include "LayoutSurface.h"

namespace Ossium
{

    BaseComponent* LayoutComponent::ComponentFactory(void* target_entity)      
    {                                                               
        return nullptr;                                             
    }                                                               
    LayoutComponent::LayoutComponent() {}
    LayoutComponent::~LayoutComponent() {}
    void LayoutComponent::OnCreate() { ParentType::OnCreate(); }
    void LayoutComponent::OnDestroy() { ParentType::OnDestroy(); }
    void LayoutComponent::OnLoadStart() { ParentType::OnLoadStart(); }
    void LayoutComponent::OnClone(BaseComponent* src) {}
    void LayoutComponent::Update(){}
    Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType> LayoutComponent::__ecs_factory_ = 
    std::is_same<ParentType, BaseComponent>::value ? Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType>(
        SID( "LayoutComponent" )::str, ComponentFactory
    ) :
    Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType>(
        SID( "LayoutComponent" )::str, ComponentFactory, std::string(parentTypeName), true
    );

    void LayoutComponent::OnLoadFinish()
    {
        layoutSurface = entity->GetComponent<LayoutSurface>();
        if (!layoutSurface)
        {
            layoutSurface = entity->GetAncestor<LayoutSurface>();
            if (!layoutSurface)
            {
                // Automagically add one.
                layoutSurface = entity->AddComponent<LayoutSurface>();
            }
        }
    }

    void LayoutComponent::LayoutSetDirty()
    {
        layoutDirty = true;
    }

    bool LayoutComponent::LayoutIsDirty()
    {
        return layoutDirty;
    }

    void LayoutComponent::LayoutUpdate()
    {
        if (layoutDirty)
        {
            layoutDirty = false;
        }
        LayoutRefresh();
        auto children = entity->GetComponentsInChildren<LayoutComponent>();
        for (auto child : children)
        {
            child->LayoutUpdate();
        }
    }

}
