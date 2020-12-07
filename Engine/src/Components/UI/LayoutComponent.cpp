#include "LayoutComponent.h"
#include "LayoutSurface.h"

using namespace std;

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
    void LayoutComponent::OnSetEnabled(bool enable) { ParentType::OnSetEnabled(enable); }
    void LayoutComponent::OnLoadStart() { ParentType::OnLoadStart(); }
    void LayoutComponent::OnClone(BaseComponent* src) {}
    void LayoutComponent::Update(){}
    std::string LayoutComponent::GetBaseTypeNames()
    {
        return std::is_same<BaseComponent, ParentType>::value ?
            std::string("") : std::string(parentTypeName) + "," + ParentType::GetBaseTypeNames();
    }
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
        // After loading, make sure the LayoutSurface is marked dirty!
        layoutSurface->SetDirty();
    }

    void LayoutComponent::OnEditorPropertyChanged()
    {
        // Mark the layout dirty.
        layoutSurface->SetDirty();
    }

    void LayoutComponent::OnSetActive(bool active)
    {
        if (active && layoutSurface)
        {
            layoutSurface->SetDirty();
        }
    }

}
