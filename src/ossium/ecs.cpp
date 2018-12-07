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

    ecs::ECS_Controller Entity::controller;

    Entity::Entity()
    {
        transform = {{0, 0}, {0, 0}, {1, 1}};
        string name = "Entity";
        self = controller.entityTree.add(name, this);
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
            self = controller.entityTree.add(name, this, parent->self);
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

    /// Defunct. Use Clone() to make explicit copies instead
    /*Entity::Entity(const Entity& copySource)
    {
        /// Add this copy to the tree
        string name = copySource.self->name;
        unsigned int treeSize = controller.entityTree.size();
        if (treeSize > 0)
        {
            name = name + " (" + ToString(treeSize) + ")";
        }
        /// This copy gets added to the tree as a sibling to the original entity
        controller.entityTree.add(name, *this, copySource.self->parent);
        /// Set a unique ID for this copy
        id = nextId;
        nextId++;
        /// Copy transform data
        transform = copySource.transform;
        /// Deep, virtual copy of components
    }*/

    /// Defunct - assignment operator no longer makes a copy
    /*void Entity::Swap(Entity& itemOne, Entity& itemTwo)
    {
        using std::swap;
        swap(itemOne.name, itemTwo.name);
        swap(itemOne.transform, itemTwo.transform);
        swap(itemOne.components, itemTwo.components);
    }*/

    /// Instead of copying, we simply return a reference to the source
    /*Entity& Entity::operator=(Entity& source)
    {
        return (Entity&)source;
    }*/

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
        controller.entityTree.remove(self);
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
        Node<Entity*>* node = controller.entityTree.find(name);
        return node != nullptr ? node->data : nullptr;
    }

    Entity* Entity::find(string name, Entity* parent)
    {
        Node<Entity*>* node = controller.entityTree.find(name, parent->self);
        return node != nullptr ? node->data : nullptr;
    }

    unsigned int Entity::GetTotalEntities()
    {
        return controller.GetTotalEntities();
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
        entityTree.InitRoot(nullptr);
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

}
