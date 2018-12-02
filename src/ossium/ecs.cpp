#include <string>
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
        #ifdef DEBUG
        onDestroyCalled = false;
        #endif // DEBUG
    }

    Component::~Component()
    {
        #ifdef DEBUG
        SDL_assert(onDestroyCalled != false);
        #endif // DEBUG
    }

    ComponentType ecs::ComponentRegistry::nextTypeIdent = 0;

}
