#include <cstdio>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL.h>

#include "basics.h"
#include "ecs.h"
#include "delta.h"

using namespace std;

namespace Ossium
{

    string GetComponentName(Uint32 id)
    {
        return typesys::TypeFactory<Component>::GetName(id);
    }

    ComponentType GetComponentType(string name)
    {
        return typesys::TypeFactory<Component>::GetId(name);
    }

    Entity::Entity(EntityComponentSystem* entity_system, Entity* parent)
    {
        controller = entity_system;
        string name = "Entity";
        if (parent != nullptr)
        {
            self = controller->entityTree.insert(name, this, parent->self);
        }
        else
        {
            self = controller->entityTree.insert(name, this);
        }
        controller->entities[self->id] = self;
        /// Set the name again, using the generated id
        name = "Entity[" + Ossium::ToString(self->id) + "]";
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
                copyComponent->OnInitGraphics(nullptr);
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
                    vector<Component*>& ecs_components = controller->components[itr->second[i]->GetType()];
                    for (auto j = ecs_components.begin(); j != ecs_components.end(); j++)
                    {
                        if (*j == itr->second[i])
                        {
                            ecs_components.erase(j);
                            break;
                        }
                    }
                    /// Now remove the component pointer from this entity's components hash and delete it
                    itr->second[i]->OnRemoveGraphics();
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

    Entity* Entity::CreateChild()
    {
        return controller->CreateEntity(this);
    }

    void Entity::FromString(string& str)
    {
        JSON data(str);
        auto entity_itr = data.find("Components");
        if (entity_itr != data.end())
        {
            JSON components_data(entity_itr->second);

            entity_itr = data.find("ID");
            if (entity_itr != data.end())
            {
                self->id = Utilities::ToInt(entity_itr->second);
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to get entity ID! Identity conflicts may occur.");
            }

            for (auto component : components_data)
            {
                ComponentType compType = 0;
                /// TODO: map compType to the type id specified in a lookup table within the JSON data
                /// to ensure component type ids are correct between versions/devices (due to static instantiation order uncertainty).
                /// NOTE: ideally don't use component type names as these may be changed at any point during project development.
                Utilities::FromString(compType, component.first);
                vector<JString> componentData = component.second.ToArray();
                vector<Component*>& compsOfType = components[compType];
                unsigned int totalComponents = compsOfType.empty() ? 0 : compsOfType.size();
                for (unsigned int i = 0, counti = componentData.empty() ? 0 : componentData.size(); i < counti; i++)
                {
                    if (i >= totalComponents)
                    {
                        if (typesys::TypeFactory<Component>::Create(compType, (void*)this) == nullptr)
                        {
                            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to add component [\"%s\" with id %d] to entity during Entity::FromString()!", GetComponentName(compType).c_str(), compType);
                            continue;
                        }
                    }
                    compsOfType[i]->FromString(componentData[i]);
                }
            }

            /// This bit of code is needed in case components are dynamically removed,
            /// in which case the ID generator values may be non-unique unless we serialise the counter.
            entity_itr = data.find("CID Gen");
            if (entity_itr != data.end())
            {
                componentCounter = Utilities::ToInt(entity_itr->second);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to get component ID generator value. Using total components instead.");
                componentCounter = components.empty() ? 0 : components.size();
            }

        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Invalid JSON string input during entity FromString() call!");
        }
    }

    string Entity::ToString()
    {
        /// TODO: improve formatting
        JSON data;
        string json_components = "{";
        for (auto itr : components)
        {
            if (json_components.length() > 1)
            {
                json_components += ", ";
            }
            json_components += "\"" + Utilities::ToString(itr.first) + "\"" + " : [";
            for (int i = 0, counti = itr.second.empty() ? 0 : itr.second.size(); i < counti; i++)
            {
                json_components += itr.second[i]->ToString();
                if (i + 1 < counti)
                {
                    json_components += ", ";
                }
            }
            json_components += "]";
        }
        json_components += "}";
        data["Name"] = GetName();
        data["ID"] = Utilities::ToString(self->id);
        data["Parent"] = Utilities::ToString(self->parent != nullptr && self->parent->data != nullptr ? self->parent->id : -1);
        data["Components"] = json_components;
        data["CID Gen"] = Utilities::ToString(componentCounter);
        return data.ToString();
    }

    void Entity::Destroy(bool immediate)
    {
        controller->DestroyEntity(this, immediate);
    }

    ///
    /// Component
    ///

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

    Uint16 Component::GetLocalID()
    {
        return localId;
    }

    string Component::GetID()
    {
        return Utilities::ToString(entity->GetID()) + "-" + Utilities::ToString(localId);
    }

    ///
    /// GraphicComponent
    ///

    inline namespace Graphics
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
            rendererInstance = renderer != nullptr ? renderer : rendererInstance;
            if (rendererInstance != nullptr)
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

    void EntityComponentSystem::DestroyEntity(Entity* entity, bool immediate)
    {
        if (entity != nullptr)
        {
            if (entity->controller == this)
            {
                if (immediate)
                {
                    delete entity;
                }
                else
                {
                    pendingDestruction.push_back(entity);
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to destroy entity but it is not managed by this entity component system instance!");
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to destroy entity but it was already destroyed.");
        }
    }

    void EntityComponentSystem::DestroyPending()
    {
        for (auto entity : pendingDestruction)
        {
            delete entity;
        }
        pendingDestruction.clear();
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

    string EntityComponentSystem::ToString()
    {
        JSON serialised;
        for (auto mappedEntity : entities)
        {
            Node<Entity*>* entity = mappedEntity.second;
            if (entity != nullptr && entity->data != nullptr)
            {
                serialised[Utilities::ToString(entity->id)] = entity->data->ToString();
            }
        }
        return "{\n\"Tree Generation\" : " + Utilities::ToString(entityTree.GetGeneration()) + "\n\"Entities\" : " + serialised.ToString() + "\n}";
    }

    void FromString(string& str)
    {
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
