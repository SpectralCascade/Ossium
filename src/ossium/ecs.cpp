#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

#include "basics.h"
#include "transform.h"
#include "ecs.h"

namespace ossium
{

    ecs::ECS_Controller* Entity::controller = nullptr;

    Entity::Entity()
    {
        transform = {{0, 0}, {0, 0}, {1, 1}};
        string name = "Entity";
        self = controller->entityTree.add(name, this);
        /// Set the name again, using the generated id
        name = "Entity[" + ToString(self->id) + "]";
        SetName(name);
    }

    Entity::Entity(Entity* parent)
    {
        if (parent != nullptr)
        {
            transform = parent->transform;
            string name = "Entity";
            self = controller->entityTree.add(name, this, parent->self);
            name = "Entity[" + ToString(self->id) + "]";
            SetName(name);
        }
        else
        {
            /// Use default constructor instead
            Entity();
        }
    }

    Entity* Entity::Clone()
    {
        Entity* entityCopy = new Entity(self->parent->data);
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
        /// Cleanup all children
        controller->entityTree.remove(self);
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

    void ecs::ECS_Info::InitECS()
    {
        if (Entity::controller == nullptr)
        {
            Entity::controller = new ecs::ECS_Controller();
        }
        else
        {
            SDL_Log("(!) Attempted to initialise ECS subsystem, but it is already initialised.");
        }
    }

    void ecs::ECS_Info::DestroyECS()
    {
        if (Entity::controller != nullptr)
        {
            delete Entity::controller;
            Entity::controller = nullptr;
        }
        else
        {
            printf("(!) Attempted to destroy ECS subsystem, but it is already destroyed.");
        }
    }

    unsigned int ecs::ECS_Info::GetTotalEntities()
    {
        return Entity::controller->GetTotalEntities();
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

    ecs::ECS_Controller::ECS_Controller()
    {
        components = new vector<Component*>[ecs::ComponentRegistry::GetTotalTypes()];
    }

    void ecs::ECS_Controller::Clear()
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

    unsigned int ecs::ECS_Controller::GetTotalEntities()
    {
        return entityTree.size();
    }

    ecs::ECS_Controller::~ECS_Controller()
    {
        for (Uint32 i = 0, counti = ecs::ComponentRegistry::GetTotalTypes(); i < counti; i++)
        {
            components[i].clear();
        }
        delete[] components;
    }

    void ecs::InitECS()
    {
        Entity::ecs_info.InitECS();
        SDL_Log("Initialised ECS subsystem with %d registered component type(s).", ecs::ComponentRegistry::GetTotalTypes());
    }

    void ecs::DestroyECS()
    {
        Entity::ecs_info.DestroyECS();
    }

}
