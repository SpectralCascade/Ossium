#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

#include "transform.h"
#include "ecs.h"

namespace ossium
{

    int Entity::nextId = 0;

    Entity::Entity()
    {
        id = nextId;
        nextId++;
        transform = {{0, 0}, {0, 0}, {1, 1}};
    }

    Entity::Entity(const Entity& copySource)
    {
        /// Same name is fine
        name = copySource.name;
        transform = copySource.transform;
        /// Set a unique ID for this copy
        id = nextId;
        nextId++;
        /// Deep, virtual copy of components
        for (auto i = copySource.components.begin(); i != copySource.components.end(); i++)
        {
            vector<Component*> copiedComponents;
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                Component* copyComponent = (*itr)->Clone();
                copyComponent->entity = this;
                copiedComponents.push_back(copyComponent);
            }
            components.insert({i->first, copiedComponents});
        }
    }

    void Entity::Swap(Entity& itemOne, Entity& itemTwo)
    {
        using std::swap;
        swap(itemOne.name, itemTwo.name);
        swap(itemOne.transform, itemTwo.transform);
        swap(itemOne.components, itemTwo.components);
    }

    Entity& Entity::operator=(Entity copySource)
    {
        Swap(*this, copySource);
        return *this;
    }

    Entity::~Entity()
    {
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
    }

    const int Entity::GetID()
    {
        return id;
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

}
