#include <string>
#include <SDL2/SDL.h>

#include "ecs.h"

namespace ossium
{

    int Entity::nextId = 0;

    Entity::Entity()
    {
        id = nextId;
        nextId++;
    }

    Entity::~Entity()
    {
        for (auto itr = components.begin(); itr != components.end(); itr++)
        {
            for (unsigned int i = 0, counti = itr->second.empty() ? 0 : itr->second.size(); i < counti; i++)
            {
                if (itr->second[i] != nullptr)
                {
                    delete itr->second[i];
                    itr->second[i] = nullptr;
                }
            }
            itr->second.clear();
        }
        components.clear();
    }

    int Entity::GetID()
    {
        return id;
    }

    string Entity::GetName()
    {
        return name;
    }

    void Entity::SetName(string newName)
    {
        name = newName;
    }

    ComponentType ecs::ComponentRegistry::nextTypeIdent = 0;

}
