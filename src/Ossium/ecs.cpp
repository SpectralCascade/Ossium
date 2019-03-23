#include <cstdio>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL.h>

#include "basics.h"
#include "transform.h"
#include "ecs.h"
#include "delta.h"

using namespace std;

namespace Ossium
{

    Entity::Entity(EntityComponentSystem* entity_system, Entity* parent)
    {
        controller = entity_system;
        string name = "Entity";
        if (parent != nullptr)
        {
            self = controller->entityTree.add(name, this, parent->self);
        }
        else
        {
            self = controller->entityTree.add(name, this);
        }
        controller->entities[self->id] = self;
        /// Set the name again, using the generated id
        name = "Entity[" + ToString(self->id) + "]";
        SetName(name);
    }

    Entity* Entity::Clone()
    {
        Entity* entityCopy = new Entity(controller, self->parent != nullptr ? self->parent->data : nullptr);
        entityCopy->self->name = self->name + " (copy)";
        for (auto i = components.begin(); i != components.end(); i++)
        {
            vector<Component*> copiedComponents;
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                Component* copyComponent = (*itr)->Clone();
                copyComponent->entity = entityCopy;
                copyComponent->OnClone();
                copiedComponents.push_back(copyComponent);
            }
            entityCopy->components.insert({i->first, copiedComponents});
        }
        return entityCopy;
    }

    Entity::~Entity()
    {
        /// Destroy all components
        for (auto itr = components.begin(); itr != components.end(); itr++)
        {
            for (unsigned int i = 0, counti = itr->second.empty() ? 0 : itr->second.size(); i < counti; i++)
            {
                if (itr->second[i] != nullptr)
                {
                    itr->second[i]->OnDestroy();
                    delete itr->second[i];
                    itr->second[i] = nullptr;
                }
            }
            itr->second.clear();
        }
        components.clear();
        /// Clean up all children
        controller->entityTree.remove(self);
        controller->entities.erase(self->id);
    }

    const int Entity::GetID()
    {
        return self->id;
    }

    void Entity::SetName(string name)
    {
        self->name == name;
    }

    string Entity::GetName()
    {
        return self->name;
    }

    Entity* Entity::GetParent()
    {
        /// Check if this is the root entity, or the parent is the root entity
        /// We can assume self is never null as self is set when the entity is added to the entity tree
        if (self->name == "" || self->parent->name == "")
        {
            return nullptr;
        }
        else
        {
            return self->parent->data;
        }
    }

    Entity* Entity::Find(string name)
    {
        Node<Entity*>* node = controller->entityTree.find(name);
        return node != nullptr ? node->data : nullptr;
    }

    Entity* Entity::Find(string name, Entity* parent)
    {
        Node<Entity*>* node = controller->entityTree.find(name, parent->self);
        return node != nullptr ? node->data : nullptr;
    }

    Component::~Component()
    {
    }

    void Component::OnCreate()
    {
    }

    void Component::OnDestroy()
    {
    }

    void Component::OnInitGraphics(Renderer* renderer, int layer)
    {
    }

    void Component::OnRemoveGraphics()
    {
    }

    void Component::OnClone()
    {
    }

    void Component::Update()
    {
    }

    Component::Component()
    {
    }

    Component::Component(const Component& copySource)
    {
        /// Don't copy anything - we only care about child class values
        /// The entity reference should be constant once created
    }

    Entity* Component::GetEntity()
    {
        return entity;
    }

    ///
    /// GraphicComponent
    ///

    inline namespace graphics
    {

        void GraphicComponent::SetRenderLayer(int layer)
        {
            if (rendererInstance != nullptr)
            {
                rendererInstance->Unregister(this, renderLayer);
                renderLayer = rendererInstance->Register(this, layer);
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
        }

        void GraphicComponent::OnDestroy()
        {
        }

        void GraphicComponent::OnClone()
        {
        }

        void GraphicComponent::Update()
        {
        }

        void GraphicComponent::OnInitGraphics(Renderer* renderer, int layer)
        {
            renderLayer = layer >= 0 ? layer : renderLayer;
            rendererInstance = renderer;
            if (renderer != nullptr)
            {
                renderLayer = rendererInstance->Register(this, renderLayer);
            }
        }

        void GraphicComponent::OnRemoveGraphics()
        {
            if (rendererInstance != nullptr)
            {
                rendererInstance->Unregister(this, renderLayer);
            }
        }

    }

    ///
    /// EntityComponentSystem
    ///

    EntityComponentSystem::EntityComponentSystem()
    {
        components = new vector<Component*>[typesys::TypeRegistry<ComponentType>::GetTotalTypes()];
    }

    void EntityComponentSystem::UpdateComponents()
    {
        for (unsigned int i = 0, counti = typesys::TypeRegistry<ComponentType>::GetTotalTypes(); i < counti; i++)
        {
            for (auto j = components[i].begin(); j != components[i].end(); j++)
            {
                /// We shouldn't need to check if the component is null - if it is we have bigger problems!
                (*j)->Update();
            }
        }
    }

    Entity* EntityComponentSystem::CreateEntity()
    {
        Entity* created = new Entity(this);
        return created;
    }

    Entity* EntityComponentSystem::CreateEntity(Entity* parent)
    {
        Entity* created = new Entity(this, parent);
        return created;
    }

    void EntityComponentSystem::DestroyEntity(Entity* entity)
    {
        if (entity != nullptr)
        {
            if (entity->controller == this)
            {
                delete entity;
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to destroy entity but the entity does not exist in this system instance!");
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to destroy entity but the entity was already destroyed.");
        }
    }

    void EntityComponentSystem::Clear()
    {
        /// Delete all entities
        vector<Node<Entity*>*>& entities = entityTree.getFlatTree();
        for (auto i = entities.begin(); i != entities.end(); i++)
        {
            if (*i != nullptr && (*i)->data != nullptr)
            {

                delete (*i)->data;
                (*i)->data = nullptr;
            }
        }
        /// Now we can safely remove all nodes from the tree and remove all components
        entityTree.clear();
        for (unsigned int i = 0, counti = typesys::TypeRegistry<ComponentType>::GetTotalTypes(); i < counti; i++)
        {
            /// No need to delete components as they are deleted when their parent entity is destroyed
            components[i].clear();
        }
    }

    unsigned int EntityComponentSystem::GetTotalEntities()
    {
        return entityTree.size();
    }

    EntityComponentSystem::~EntityComponentSystem()
    {
        for (Uint32 i = 0, counti = typesys::TypeRegistry<ComponentType>::GetTotalTypes(); i < counti; i++)
        {
            components[i].clear();
        }
        delete[] components;
    }

}
