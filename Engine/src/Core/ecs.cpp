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
#include "resourcecontroller.h"
#include "component.h"

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

    Entity::Entity(Scene* entity_system, Node<Entity*>* node)
    {
        controller = entity_system;
        self = node;
        /// Set the name again, using the generated id
        name = "Entity[" + Ossium::ToString(self->id) + "]";
    }

    Entity* Entity::Clone(Entity* parent)
    {
        Entity* entityCopy = controller->CreateEntity(parent != nullptr ? parent : (self->parent != nullptr ? self->parent->data : nullptr));
        entityCopy->name = name + " (copy)";
        if (!active)
        {
            entityCopy->SetActive(false);
        }
        for (auto i = components.begin(); i != components.end(); i++)
        {
            vector<BaseComponent*> copiedComponents;
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                BaseComponent* copyComponent = (*itr)->Clone();
                copyComponent->entity = entityCopy;
                copyComponent->OnClone(*itr);
                copiedComponents.push_back(copyComponent);
                GetScene()->components[copyComponent->GetType()].push_back(copyComponent);
            }
            entityCopy->components.insert({i->first, copiedComponents});
            for (auto itr : copiedComponents)
            {
                itr->OnCreate();
            }
        }

        // Now clone all children.
        /*for (auto child : self->children)
        {
            child->data->Clone(this);
        }*/

        // Now call OnLoadFinish(), having created all the components on this entity.
        for (auto i = entityCopy->components.begin(); i != entityCopy->components.end(); i++)
        {
            for (auto itr = i->second.begin(); itr != i->second.end(); itr++)
            {
                (*itr)->OnLoadFinish();
            }
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
    }

    BaseComponent* Entity::AddComponent(ComponentType type)
    {
        if (WillBeDestroyed())
        {
            Log.Warning("Failed to add component! You cannot add a component to an entity that is being destroyed.");
            return nullptr;
        }
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

    void Entity::SetActiveInScene()
    {
        if (!controller->IsActive(this))
        {
            controller->SetActive(this);
            for (auto child : self->children)
            {
                child->data->SetActiveInScene();
            }
            OnSetActive(true);
        }
    }

    void Entity::SetInactiveInScene()
    {
        if (controller->IsActive(this))
        {
            controller->SetInactive(this);
            for (auto child : self->children)
            {
                child->data->SetInactiveInScene();
            }
            OnSetActive(false);
        }
    }

    void Entity::OnSetActive(bool active)
    {
        for (auto itr : components)
        {
            for (auto component : itr.second)
            {
                component->OnSetActive(active);
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

    Entity* Entity::Find(string entityName, Entity* parent)
    {
        Node<Entity*>* node = controller->entityTree.Find(
            [entityName] (Node<Entity*>* n) { return n->data->name == entityName; },
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

    bool Entity::WillBeDestroyed()
    {
        return controller->pendingDestruction.find(this) != controller->pendingDestruction.end();
    }

    Scene* Entity::GetScene()
    {
        return controller;
    }

    void Entity::SetScene(Scene* scene, Entity* parent)
    {
        if (controller != scene)
        {
            auto oldSelf = self;
            Scene* oldScene = controller;

            // Insert nodes into the destination scene.
            oldScene->entityTree.WalkBreadth([scene, parent, oldScene, this] (Node<Entity*>* node) {
                Entity* walked = node->data;

                // Keep active state
                if (!walked->IsActive())
                {
                    scene->inactiveEntities.insert(walked);
                }
                
                // Erase references in the source scene.
                oldScene->inactiveEntities.erase(walked);
                oldScene->pendingDestruction.erase(walked);
                oldScene->entities.erase(walked->self->id);
                for (auto itr : walked->components)
                {
                    for (auto c : itr.second)
                    {
                        oldScene->pendingDestructionComponents.erase(c);
                        for (auto i = oldScene->components[itr.first].begin(); i != oldScene->components[itr.first].end(); i++)
                        {
                            if (*i == c)
                            {
                                oldScene->components[itr.first].erase(i);
                                break;
                            }
                        }
                        // While here, add components to the destination scene.
                        scene->components[itr.first].push_back(c);
                    }
                }

                // Insert nodes into destination scene hierarchy.
                auto previousSelf = walked->self;
                if (this == walked)
                {
                    walked->self = scene->entityTree.Insert(
                        walked,
                        parent != nullptr && parent->GetScene() == scene ? parent->self : nullptr
                    );
                }
                else
                {
                    walked->self = scene->entityTree.Insert(
                        walked,
                        walked->self->parent
                    );
                }
                scene->entities[walked->self->id] = walked->self;

                // After inserting, update parenting of all children.
                // A little cheeky but safe as these will be removed anyway later.
                for (auto child : previousSelf->children)
                {
                    child->parent = walked->self;
                }

                walked->controller = scene;
                // Figure out why this is necessary. Looks like stuff isn't actually cleaned up...
                previousSelf->data = nullptr;

                return true;
            }, oldSelf);

            // Remove root references from the source scene.
            oldScene->entityTree.Remove(oldSelf);

            // TODO: OnSceneChanged() callback for components perhaps?
        }
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

    void BaseComponent::OnSetActive(bool active)
    {
    }

    void BaseComponent::OnSetEnabled(bool active)
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

    void BaseComponent::OnEditorPropertyChanged()
    {
    }

    void BaseComponent::Update()
    {
    }

    string BaseComponent::GetBaseTypeNames()
    {
        return string();
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
        return IsEnabled() && entity->IsActive();
    }

    bool BaseComponent::IsEnabled()
    {
        // Use reflection to get the private "enabled" member.
        return *((bool*)GetMember(0));
    }

    void BaseComponent::SetEnabled(bool enable)
    {
        // Use reflection to get the private "enabled" member and set it.
        *((bool*)GetMember(0)) = enable;
        OnSetEnabled(enable);
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
        string toParse = Utilities::FileToString(guid_path);
        if (toParse.empty())
        {
            return false;
        }
        this->guid_path = guid_path;
        FromString(toParse);
        onLoadComplete(this);
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

    string Scene::GetFilePath()
    {
        return guid_path;
    }

    void Scene::LoadSafe(string guid_path)
    {
        toLoad = guid_path;
    }

    void Scene::ClearSafe()
    {
        clearPostUpdate = true;
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
        // Should be safe to clean up components now. Assuming they clean up after themselves!
        if (!toLoad.empty())
        {
            Load(toLoad);
            clearPostUpdate = false;
            toLoad = "";
        }
        else if (clearPostUpdate)
        {
            Clear();
        }
    }

    Entity* Scene::CreateEntity(Entity* parent)
    {
        Node<Entity*>* node = nullptr;
        if (parent != nullptr)
        {
            node = entityTree.Insert(nullptr, parent->self);
        }
        else
        {
            node = entityTree.Insert(nullptr);
        }
        Entity* created = new Entity(this, node);
        node->data = created;
        entities[node->id] = node;
        return created;
    }

    void Scene::DestroyEntity(Entity* entity, bool immediate)
    {
        if (entity == nullptr)
        {
            Log.Warning("Attempted to destroy entity but it was already destroyed.");
        }
        else if (entity->controller != this)
        {
            Log.Warning("Attempted to destroy entity but it is not managed by this scene instance!");
        }
        else if (immediate)
        {
            auto rootNode = entity->self;
            // Clean up all children first; only delete entities on way back up the tree
            // so the parent hierarchy is not broken during destruction.
            entityTree.Walk(
                [&] (Node<Entity*>* node) {
                    // Indicate that entities are in the process of being destroyed
                    pendingDestruction.insert(node->data);
                    return true;
                },
                [&] (Node<Entity*>* node) {
                    if (node->data == nullptr)
                    {
                        Log.Error("Invalid entity in scene! Node [{0}] at depth {1}", node, node->depth);
                    }

                    Log.Debug("Deleting entity at {0} node [{1}]", node->data, node);
                    Log.Debug("Entity name: {0}", node->data->name);

                    // Destroy the entity and it's components.
                    delete node->data;

                    // Cleanup pending destruction
                    if (pendingDestruction.find(node->data) != pendingDestruction.end())
                    {
                        pendingDestruction.erase(node->data);
                    }

                    // Cleanup everything else
                    auto itr = inactiveEntities.find(node->data);
                    if (itr != inactiveEntities.end())
                    {
                        inactiveEntities.erase(itr);
                    }
                    entities.erase(node->id);
                    node->data = nullptr;
                    return true;
                },
                entity->self
            );
            entityTree.Remove(rootNode);
        }
        else
        {
            pendingDestruction.insert(entity);
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
                component->OnDestroy();
                /// Now remove the component pointer from the entity's components hash and delete the component.
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
        // Do components first. Use this looping style to ensure the iterator is not invalidated.
        for (auto itr = pendingDestructionComponents.begin(); itr != pendingDestructionComponents.end(); itr = pendingDestructionComponents.begin())
        {
            DestroyComponent(*itr, true);
        }
        pendingDestructionComponents.clear();
        // Now entities, same looping style for same reason.
        for (auto itr = pendingDestruction.begin(); itr != pendingDestruction.end(); itr = pendingDestruction.begin())
        {
            DestroyEntity(*itr, true);
        }
        pendingDestruction.clear();
    }

    void Scene::Clear()
    {
        /// Delete all entities
        WalkEntities([&] (Entity* entity) {
            DestroyEntity(entity, true);
            // Only destroy the root entities; all their children are automagically destroyed.
            return false;
        });
        /// Now we can safely remove all nodes from the tree and remove all components
        entities.clear();
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
        JSON serialised;
        // Walk through the entity tree
        WalkEntities(
            [&] (Entity* entity) {
                if (entity != nullptr)
                {
                    serialised[Utilities::ToString(entity->self->id)] = entity->ToString();
                }
                return true;
            }
        );
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
            //Log.Info("Loading components of type {0}", GetComponentName(i));
            int total = 0;
            for (auto component : components[i])
            {
                Log.Debug("Finished loading \"{0}\" component on entity \"{1}\"", GetComponentName(component->GetType()), component->entity->name);
                component->OnLoadFinish();
                total++;
            }
            //Log.Info("Loaded {0} component(s) of type {1}", total, GetComponentName(i));
        }

#ifdef OSSIUM_DEBUG
        if (entities.size() != serialised.size())
        {
            Log.Warning("Serialised entities ({0}) != created entities ({1})!", entities.size(), serialised.size());
        }
#endif
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

    ServicesProvider* Scene::GetServices()
    {
        return servicesProvider;
    }

    void Scene::WalkEntities(function<bool(Entity*)> walkFunc, bool breadthFirst, Entity* startEntity)
    {
        if (breadthFirst)
        {
            entityTree.WalkBreadth([&walkFunc] (Node<Entity*>* node) {
                if (node->data != nullptr)
                {
                    return walkFunc(node->data);
                }
                return true;
            }, startEntity != nullptr ? startEntity->self : nullptr);
        }
        else
        {
            entityTree.Walk([&walkFunc] (Node<Entity*>* node) {
                if (node->data != nullptr)
                {
                    return walkFunc(node->data);
                }
                return true;
            }, startEntity != nullptr ? startEntity->self : nullptr);
        }

    }

    Entity* Scene::Find(string entityName, Entity* parent)
    {
        if (parent == nullptr)
        {
            auto found = entityTree.Find([=] (Node<Entity*>* n) { return n->data->name == entityName; }, parent != nullptr ? parent->self : nullptr);
            return found != nullptr ? found->data : nullptr;
        }
        return parent->Find(entityName);
    }

    Scene::~Scene()
    {
        Clear();
        delete[] components;
        components = nullptr;
    }

}
