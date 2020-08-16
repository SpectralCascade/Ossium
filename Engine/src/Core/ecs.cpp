/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include <cstdio>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include "funcutils.h"
#include "stringconvert.h"
#include "ecs.h"
#include "delta.h"
#include "engineconstants.h"

using namespace std;

namespace Ossium
{

    string GetComponentName(Uint32 id)
    {
        return TypeSystem::TypeFactory<BaseComponent, ComponentType>::GetName(id);
    }

    ComponentType GetComponentType(string name)
    {
        return TypeSystem::TypeFactory<BaseComponent, ComponentType>::GetId(name);
    }

    bool IsAbstractComponent(Uint32 id)
    {
        return TypeSystem::TypeFactory<BaseComponent, ComponentType>::IsAbstract(id);
    }

    Uint32 GetTotalComponentTypes()
    {
        return TypeSystem::TypeFactory<BaseComponent, ComponentType>::GetTotalTypes();
    }

    Entity::Entity(Scene* entity_system, Entity* parent)
    {
        controller = entity_system;
        if (parent != nullptr)
        {
            self = controller->entityTree.Insert(this, parent->self);
        }
        else
        {
            self = controller->entityTree.Insert(this);
        }
        controller->entities[self->id] = self;
        /// Set the name again, using the generated id
        name = "Entity[" + Ossium::ToString(self->id) + "]";
    }

    Entity* Entity::Clone()
    {
        Entity* entityCopy = new Entity(controller, self->parent != nullptr ? self->parent->data : nullptr);
        entityCopy->name = name + " (copy)";
        for (auto i = components.begin(); i != components.end(); i++)
        {
            vector<BaseComponent*> copiedComponents;
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                BaseComponent* copyComponent = (*itr)->Clone();
                copyComponent->entity = entityCopy;
                copyComponent->OnClone(*itr);
                copiedComponents.push_back(copyComponent);
            }
            entityCopy->components.insert({i->first, copiedComponents});
        }
        /// Trigger active state setup
        entityCopy->OnSceneLoaded();

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
                    itr->second[i]->Destroy(true);
                }
            }
            itr->second.clear();
        }
        components.clear();

        /// Clean up all children
        for (auto node : self->children)
        {
            if (node->data != nullptr)
            {
                controller->DestroyEntity(node->data, true);
            }
        }

        controller->entityTree.Remove(self);
        /// Remove this instance from inactive set and the entities map
        auto itr = controller->inactiveEntities.find(this);
        if (itr != controller->inactiveEntities.end())
        {
            controller->inactiveEntities.erase(itr);
        }
        controller->entities.erase(self->id);
    }

    BaseComponent* Entity::AddComponent(ComponentType type)
    {
        return TypeSystem::TypeFactory<BaseComponent, ComponentType>::Create(type, (void*)this);
    }

    BaseComponent* Entity::AddComponentOnce(ComponentType type)
    {
        BaseComponent* component = GetComponent(type);
        if (component == nullptr)
        {
            component = AddComponent(type);
        }
        return component;
    }

    BaseComponent* Entity::GetComponent(ComponentType type)
    {
        return components[type].empty() ? nullptr : components[type][0];
    }

    vector<BaseComponent*>& Entity::GetComponents(ComponentType compType)
    {
        return components[compType];
    }

    std::unordered_map<ComponentType, std::vector<BaseComponent*>>& Entity::GetAllComponents()
    {
        return components;
    }

    const int Entity::GetID()
    {
        return self->id;
    }

    string Entity::GetReferenceID()
    {
        return Utilities::ToString(self->id);
    }

    bool Entity::IsActive()
    {
        return controller->IsActive(this);
    }

    bool Entity::IsActiveLocally()
    {
        return active;
    }

    void Entity::SetActive(bool activate)
    {
        /// Only bother setting stuff if there is an actual change.
        if (active != activate)
        {
            active = activate;
            if (active)
            {
                SetActiveInScene();
            }
            else
            {
                SetInactiveInScene();
            }
        }
    }

    void Entity::SetActiveInScene()
    {
        /// If LOCALLY active... otherwise short circuit out,
        /// we shouldn't set this or it's children active in the scene.
        if (active)
        {
            controller->SetActive(this);
            for (auto child : self->children)
            {
                child->data->SetActiveInScene();
            }
        }
    }

    void Entity::SetInactiveInScene()
    {
        /// If LOCALLY active... otherwise short circuit out,
        /// we must already be inactive in the scene, so no need to go on.
        if (active)
        {
            controller->SetInactive(this);
            for (auto child : self->children)
            {
                child->data->SetInactiveInScene();
            }
        }
    }

    void Entity::MapReference(string ident, void** ptr)
    {
        controller->serialised_pointers[ident].insert(ptr);
    }

    Entity* Entity::GetParent()
    {
        /// Check if this is the root entity, or the parent is the root entity
        /// We can assume self is never null as self is set when the entity is added to the entity tree
        if (self->parent == nullptr || name.empty() || self->parent->data->name.empty())
        {
            return nullptr;
        }
        else
        {
            return self->parent->data;
        }
    }

    Entity* Entity::Find(string targetName, Entity* parent = nullptr)
    {
        Node<Entity*>* node = controller->entityTree.Find(
            [targetName] (Node<Entity*>* n) { return n->data->name == targetName; },
            parent != nullptr ? parent->self : nullptr
        );
        return node != nullptr ? node->data : nullptr;
    }

    Entity* Entity::CreateChild()
    {
        return controller->CreateEntity(this);
    }

    void Entity::SetParent(Entity* parent)
    {
        controller->entityTree.SetParent(self, parent != nullptr ? parent->self : nullptr);
    }

    void Entity::OnSceneLoaded()
    {
        if (!active)
        {
            /// Bypass changes check
            active = true;
            SetActive(false);
        }
    }

    void Entity::FromString(const string& str)
    {
        JSON data(str);

        auto entity_itr = data.find("Name");
        if (entity_itr != data.end())
        {
            name = entity_itr->second;
        }
        else
        {
            Log.Error("Failed to get entity name!");
        }

        entity_itr = data.find("Active");
        if (entity_itr != data.end())
        {
            Utilities::FromString(active, (string)entity_itr->second);
        }
        else
        {
            Log.Warning("Failed to get entity active flag!");
        }

        entity_itr = data.find("Components");
        if (entity_itr != data.end())
        {
            JSON components_data(entity_itr->second);

            for (auto component : components_data)
            {
                ComponentType compType = 0;
                compType = GetComponentType(component.first);
                if (!TypeSystem::TypeRegistry<BaseComponent>::IsValidType(compType))
                {
                    Log.Error("Failed to add component of type \"{0}\" [{1}] due to invalid type!", component.first, compType);
                    continue;
                }
                //Log.Info("Creating component of type \"{0}\" [{1}]", component.first, compType);
                vector<JString> componentData = component.second.ToArray();
                vector<BaseComponent*>& compsOfType = components[compType];
                unsigned int totalComponents = compsOfType.empty() ? 0 : compsOfType.size();
                for (unsigned int i = 0, counti = componentData.empty() ? 0 : componentData.size(); i < counti; i++)
                {
                    BaseComponent* comp = nullptr;
                    if (i >= totalComponents)
                    {
                        comp = AddComponent(compType);
                        if (comp == nullptr)
                        {
                            Log.Error("Failed to add component of type \"{0}\" [{1}] to entity during Entity::FromString()!", component.first, compType);
                            continue;
                        }
                        totalComponents++;
                    }
                    else
                    {
                        comp = compsOfType[i];
                    }
                    comp->OnLoadStart();
                    comp->FromString(componentData[i]);
                }
            }

        }
        else
        {
            Log.Error("Invalid JSON string input during entity FromString() call!");
        }
    }

    string Entity::ToString()
    {
        JSON data;
        JSON json_components;
        for (auto itr : components)
        {
            vector<JString> component_array;
            for (int i = 0, counti = itr.second.empty() ? 0 : itr.second.size(); i < counti; i++)
            {
                component_array.push_back(itr.second[i]->ToString());
            }
            if (!component_array.empty())
            {
                json_components[GetComponentName((ComponentType)itr.first)] = Utilities::ToString(component_array);
            }
        }
        data["Name"] = name;
        data["Active"] = Utilities::ToString(active);
        data["Parent"] = Utilities::ToString(self->parent != nullptr && self->parent->data != nullptr ? self->parent->id : -1);
        data["Components"] = json_components.ToString();
        return data.ToString();
    }

    void Entity::Destroy(bool immediate)
    {
        controller->DestroyEntity(this, immediate);
    }

    int Entity::GetDepth()
    {
        return self->depth;
    }

    Scene* Entity::GetScene()
    {
        return controller;
    }

    ///
    /// BaseComponent
    ///

    BaseComponent::~BaseComponent()
    {
    }

    void BaseComponent::OnCreate()
    {
    }

    void BaseComponent::OnDestroy()
    {
    }

    void BaseComponent::OnLoadStart()
    {
    }

    void BaseComponent::OnLoadFinish()
    {
    }

    void BaseComponent::OnClone(BaseComponent* src)
    {
    }

    void BaseComponent::Update()
    {
    }

    BaseComponent::BaseComponent()
    {
    }

    BaseComponent::BaseComponent(const BaseComponent& copySource)
    {
        /// Don't copy anything - we only care about child class values
        /// The entity reference should be constant once created
    }

    Entity* BaseComponent::GetEntity()
    {
        return entity;
    }

    string BaseComponent::GetReferenceID()
    {
        ComponentType compType = GetType();
        Entity* parentEntity = GetEntity();
        vector<BaseComponent*>& entComps = parentEntity->GetComponents(compType);
        for (unsigned int i = 0, counti = entComps.empty() ? 0 : entComps.size(); i < counti; i++)
        {
            if (entComps[i] == this)
            {
                return Utilities::ToString(parentEntity->GetID())
                         + ":" + GetComponentName(compType) + ":" + Utilities::ToString(i);
            }
        }
        return string("null");
    }

    bool BaseComponent::IsActiveAndEnabled()
    {
        return enabled && entity->IsActive();
    }

    void BaseComponent::Destroy(bool immediate)
    {
        entity->GetScene()->DestroyComponent(this, immediate);
    }

    ///
    /// Scene
    ///

    REGISTER_RESOURCE(Scene);

    Scene::Scene(ServicesProvider* services)
    {
        servicesProvider = services;
        components = new vector<BaseComponent*>[TypeSystem::TypeRegistry<BaseComponent>::GetTotalTypes()];
    }

    bool Scene::Load(string guid_path)
    {
        ifstream file(guid_path.c_str());
        string toParse = Utilities::FileToString(file);
        file.close();
        if (toParse.empty())
        {
            return false;
        }
        FromString(toParse);
        return true;
    }

    bool Scene::Init(ServicesProvider* services)
    {
        servicesProvider = services;
        return true;
    }

    bool Scene::LoadAndInit(string guid_path, ServicesProvider* services)
    {
        // Actually initialise before loading in this case; such that services are available to loaded entities.
        return Init(services) && Load(guid_path);
    }

    bool Scene::Save(string directoryPath)
    {
        ofstream file(directoryPath);
        if (file.is_open())
        {
            file << ToString();
            file.close();
            return true;
        }
        return false;
    }

    void Scene::SetName(string name)
    {
        name = Utilities::SanitiseFilename(name);
        if (!name.empty())
        {
            this->name = name;
        }
        else
        {
            Log.Warning("Scene name empty or invalid characters were used, cannot set name to '{0}'.", name);
        }
    }

    string Scene::GetName()
    {
        return name;
    }

    void Scene::UpdateComponents()
    {
        for (unsigned int i = 0, counti = TypeSystem::TypeRegistry<BaseComponent>::GetTotalTypes(); i < counti; i++)
        {
            for (auto j = components[i].begin(); j != components[i].end(); j++)
            {
                if ((*j)->IsActiveAndEnabled())
                {
                    (*j)->Update();
                }
            }
        }
    }

    Entity* Scene::Find(std::string entityName)
    {
        Node<Entity*>* node = entityTree.Find([&entityName](Node<Entity*>* n){ return n->data->name == entityName; });
        return node != nullptr ? node->data : nullptr;
    }

    Entity* Scene::CreateEntity()
    {
        Entity* created = new Entity(this);
        return created;
    }

    Entity* Scene::CreateEntity(Entity* parent)
    {
        Entity* created = new Entity(this, parent);
        return created;
    }

    void Scene::DestroyEntity(Entity* entity, bool immediate)
    {
        if (entity != nullptr)
        {
            if (entity->controller == this)
            {
                if (immediate)
                {
                    // Entity could be pending destruction already
                    if (pendingDestruction.find(entity) != pendingDestruction.end())
                    {
                        pendingDestruction.erase(entity);
                    }
                    delete entity;
                }
                else
                {
                    pendingDestruction.insert(entity);
                }
            }
            else
            {
                Log.Warning("Attempted to destroy entity but it is not managed by this entity component system instance!");
            }
        }
        else
        {
            Log.Warning("Attempted to destroy entity but it was already destroyed.");
        }
    }

    void Scene::DestroyComponent(BaseComponent* component, bool immediate)
    {
        if (immediate)
        {
            // May already have listed component for destruction
            if (pendingDestructionComponents.find(component) != pendingDestructionComponents.end())
            {
                pendingDestructionComponents.erase(component);
            }

            auto itr = component->entity->GetAllComponents().find(component->GetType());
            if (itr != component->entity->GetAllComponents().end() && !itr->second.empty() && itr->second[0] != nullptr)
            {
                std::vector<BaseComponent*>& ecs_components = components[component->GetType()];
                /// First, remove the component pointer from the Scene
                for (auto i = ecs_components.begin(); i != ecs_components.end(); i++)
                {
                    if (*i == component)
                    {
                        ecs_components.erase(i);
                        break;
                    }
                }
                /// Now remove the component pointer from the entity's components hash and delete the component.
                component->OnDestroy();
                itr->second.erase(itr->second.begin());
                delete component;
            }
            else
            {
                // Must be destroyed already.
                Log.Warning("Failed to locate component on entity '{0}' while attempting to manually destroy component. You should not call Destroy() more than once!", component->entity->name);
            }
        }
        else
        {
            pendingDestructionComponents.insert(component);
        }
    }

    void Scene::DestroyPending()
    {
        // Do components first.
        for (auto component : pendingDestructionComponents)
        {
            DestroyComponent(component, true);
        }
        pendingDestructionComponents.clear();
        // Now entities.
        for (auto entity : pendingDestruction)
        {
            // Entities have a proper destructor.
            delete entity;
        }
        pendingDestruction.clear();
    }

    void Scene::Clear()
    {
        /// Delete all entities
        vector<Node<Entity*>*>& entities = entityTree.GetFlatTree();
        for (auto i = entities.begin(); i != entities.end(); i++)
        {
            if (*i != nullptr && (*i)->data != nullptr)
            {

                delete (*i)->data;
                (*i)->data = nullptr;
            }
        }
        /// Now we can safely remove all nodes from the tree and remove all components
        entityTree.Clear();
        for (unsigned int i = 0, counti = TypeSystem::TypeRegistry<BaseComponent>::GetTotalTypes(); i < counti; i++)
        {
            /// No need to delete components as they are deleted when their parent entity is destroyed
            components[i].clear();
        }
    }

    unsigned int Scene::GetTotalEntities()
    {
        return entityTree.Size();
    }

    void Scene::SetActive(Entity* entity)
    {
        auto itr = inactiveEntities.find(entity);
        if (itr != inactiveEntities.end())
        {
            inactiveEntities.erase(itr);
        }
    }

    void Scene::SetInactive(Entity* entity)
    {
        inactiveEntities.insert(entity);
    }

    bool Scene::IsActive(Entity* entity)
    {
        return inactiveEntities.find(entity) == inactiveEntities.end();
    }

    string Scene::ToString()
    {
        // TODO: maintain entity order
        JSON serialised;
        for (auto mappedEntity : entities)
        {
            Node<Entity*>* entity = mappedEntity.second;
            if (entity != nullptr && entity->data != nullptr)
            {
                serialised[Utilities::ToString(entity->id)] = entity->data->ToString();
            }
        }
        return serialised.ToString();
    }

    void Scene::FromString(const string& str)
    {
        Clear();
        JSON serialised(str);
        /// Map of entities that need to be nested under a parent entity
        vector<pair<Entity*, int>> parentMap;
        for (auto itr : serialised)
        {
            if (!IsInt(itr.first))
            {
                Log.Error("Failed to load entity due to invalid ID '{0}'!", itr.first);
                continue;
            }
            int id = ToInt(itr.first);
            if (id >= entityTree.GetGeneration())
            {
                /// TODO: make this more robust?
                /// Make sure the tree always generates unique ids
                entityTree.SetGeneration(id + 1);
            }
            Entity* entity = CreateEntity();

            /// Replace generated id with serialised id
            entities.erase(entity->self->id);
            entity->self->id = id;
            entities[id] = entity->self;

            entity->FromString(itr.second);
            JSON serialisedEntity(itr.second);
            auto parentItr = serialisedEntity.find("Parent");
            if (parentItr != serialisedEntity.end())
            {
                int ident = IsInt(parentItr->second) ? ToInt(parentItr->second) : -1;
                if (ident >= 0)
                {
                    pair<Entity*, int> parentPair(entity, ToInt(parentItr->second));
                    parentMap.push_back(parentPair);
                }
            }
            else
            {
                Log.Warning("Failed to get entity parent!");
            }
        }
        /// Now setup the entity hierarchy
        for (auto itr : parentMap)
        {
            auto entityItr = entities.find(itr.second);
            if (entityItr != entities.end())
            {
                itr.first->SetParent(entityItr->second->data);
            }
            else
            {
                Log.Warning("Entity with id '{0}' does not exist in this ECS!", itr.second);
            }
        }
        /// Finally, hook up the serialised pointers
        for (auto itr : serialised_pointers)
        {
            if (IsInt(itr.first))
            {
                auto entityItr = entities.find(ToInt(itr.first));
                if (entityItr != entities.end())
                {
                    for (void** i : itr.second)
                    {
                        *i = ((void*)entityItr->second->data);
                    }
                }
                else
                {
                    Log.Warning("Could not find entity with id '{0}'.", itr.first);
                }
            }
            else
            {
                string ent_id = SplitLeft(itr.first, ':', "error");
                if (IsInt(ent_id))
                {
                    /// Must be a component pointer
                    auto entityItr = entities.find(ToInt(ent_id));
                    if (entityItr != entities.end())
                    {
                        string comp_type = SplitLeft(SplitRight(itr.first, ':', "error"), ':', "error");
                        ComponentType compTypeId = GetComponentType(comp_type);
                        if (TypeSystem::TypeRegistry<BaseComponent>::IsValidType(compTypeId))
                        {
                            vector<BaseComponent*>& comps = entityItr->second->data->components[compTypeId];
                            string compid = SplitRight(SplitRight(itr.first, ':', "error"), ':', "error");
                            if (IsInt(compid) && !comps.empty())
                            {
                                unsigned int id = ToInt(compid);
                                if (comps.size() >= id)
                                {
                                    for (void** i : itr.second)
                                    {
                                        *i = ((void*)comps[id]);
                                    }
                                }
                                else
                                {
                                    Log.Warning("Could not find component '{0}'.", itr.first);
                                }
                            }
                            else
                            {
                                Log.Warning("Could not find component of type \"{0}\" [{1}] with index '{2}'.", comp_type, compTypeId, compid);
                            }
                        }
                        else
                        {
                            Log.Warning("Could not find component due to invalid type '{0}'.", comp_type);
                        }
                    }
                    else
                    {
                        Log.Warning("Could not find entity using component id '{0}'.", itr.first);
                    }
                }
                else
                {
                    Log.Warning("Failed to extract entity id from key '{0}'.", itr.first);
                }
            }
        }
        serialised_pointers.clear();

        /// Notify all entities that the scene has finished loading
        for (auto entityNode : entityTree.GetFlatTree())
        {
            entityNode->data->OnSceneLoaded();
        }
        /// Notify all components that the scene has finished loading
        for (unsigned int i = 0, counti = TypeSystem::TypeRegistry<BaseComponent>::GetTotalTypes(); i < counti; i++)
        {
            for (auto component : components[i])
            {
                component->OnLoadFinish();
            }
        }

        DEBUG_ASSERT(entities.size() == serialised.size(), "Input entities != created entities!");
    }

    vector<Entity*> Scene::GetRootEntities()
    {
        vector<Entity*> roots;
        for (auto node : entityTree.GetRoots())
        {
            roots.push_back(node->data);
        }
        return roots;
    }

    void Scene::WalkEntities(function<void(Entity*)> walkFunc, bool breadthFirst)
    {
        if (breadthFirst)
        {
            entityTree.WalkBreadth([&walkFunc] (Node<Entity*>* node) {
                if (node->data != nullptr)
                {
                    walkFunc(node->data);
                }
            });
        }
        else
        {
            entityTree.Walk([&walkFunc] (Node<Entity*>* node) {
                if (node->data != nullptr)
                {
                    walkFunc(node->data);
                }
            });
        }

    }

    Scene::~Scene()
    {

        entityTree.Walk([&] (auto node) { }, [&] (auto node) { if (node->data != nullptr) { node->data->Destroy(); } });
        DestroyPending();

        for (Uint32 i = 0, counti = TypeSystem::TypeRegistry<BaseComponent>::GetTotalTypes(); i < counti; i++)
        {
            components[i].clear();
        }
        delete[] components;
        components = nullptr;
    }

}
