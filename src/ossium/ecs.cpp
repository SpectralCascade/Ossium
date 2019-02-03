#include <cstdio>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL.h>

#include "basics.h"
#include "transform.h"
#include "ecs.h"

using namespace std;

namespace ossium
{

    Entity::Entity(EntityComponentSystem* entity_system, Entity* parent)
    {
        controller = entity_system;
        string name = "Entity";
        if (parent != nullptr)
        {
            transform = parent->transform;
            self = controller->entityTree.add(name, this, parent->self);
        }
        else
        {
            transform = {{0, 0}, {0, 0}, {1, 1}};
            self = controller->entityTree.add(name, this);
        }
        controller->entities[self->id] = self;
        /// Set the name again, using the generated id
        name = "Entity[" + ToString(self->id) + "]";
        SetName(name);
    }

    Entity* Entity::Clone()
    {
        Entity* entityCopy = new Entity(controller, self->parent->data);
        entityCopy->self->name = self->name + " (copy)";
        for (auto i = components.begin(); i != components.end(); i++)
        {
            vector<Component*> copiedComponents;
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                Component* copyComponent = (*itr)->Clone();
                copyComponent->entity = entityCopy;
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

    Entity* Entity::find(string name)
    {
        Node<Entity*>* node = controller->entityTree.find(name);
        return node != nullptr ? node->data : nullptr;
    }

    Entity* Entity::find(string name, Entity* parent)
    {
        Node<Entity*>* node = controller->entityTree.find(name, parent->self);
        return node != nullptr ? node->data : nullptr;
    }

    void Component::OnCreate()
    {
    }

    void Component::OnDestroy()
    {
        #ifdef DEBUG
        onDestroyCalled = true;
        #endif // DEBUG
    }

    void Component::OnSpawn()
    {
    }

    void Component::Update()
    {
    }

    Component::Component()
    {
        entity = nullptr;
        #ifdef DEBUG
        onDestroyCalled = false;
        #endif // DEBUG
    }

    Component::Component(const Component& copySource)
    {
        /// Don't copy anything - we only care about child class values
        /// The entity reference should be constant once created
    }

    Component& Component::operator=(const Component& copySource)
    {
        /// Ditto
        return *this;
    }

    Entity* Component::GetEntity()
    {
        return entity;
    }

    Component::~Component()
    {
        #ifdef DEBUG
        SDL_assert(onDestroyCalled != false);
        #endif // DEBUG
    }

    ComponentType ecs::ComponentRegistry::nextTypeIdent = 0;

    EntityComponentSystem::EntityComponentSystem()
    {
        components = new vector<Component*>[ecs::ComponentRegistry::GetTotalTypes()];
    }

    void EntityComponentSystem::UpdateComponents()
    {
        for (unsigned int i = 0, counti = ecs::ComponentRegistry::GetTotalTypes(); i < counti; i++)
        {
            for (auto j = components[i].begin(); j != components[i].end(); j++)
            {
                /// We shouldn't need to check if the component is null - if it is we have bigger problems!
                (*j)->Update();
            }
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
        for (unsigned int i = 0, counti = ecs::ComponentRegistry::GetTotalTypes(); i < counti; i++)
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
        for (Uint32 i = 0, counti = ecs::ComponentRegistry::GetTotalTypes(); i < counti; i++)
        {
            components[i].clear();
        }
        delete[] components;
    }

}
