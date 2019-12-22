/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#pragma once

#ifndef ECS_H
#define ECS_H

#include <vector>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

#include "tree.h"
#include "stringintern.h"
#include "schemamodel.h"
#include "services.h"

using namespace std;

namespace Ossium
{
    typedef Uint32 ComponentType;

    /// Declares a component type, declares a virtual copy method and constructor
    /// Add this to the end of any class you wish to register as a component
    #define DECLARE_COMPONENT(TYPE)                                                     \
    friend class Ossium::Entity;                                                        \
    protected:                                                                          \
        virtual TYPE* Clone();                                                          \
                                                                                        \
        TYPE(){};                                                                       \
                                                                                        \
        TYPE& operator=(const Entity& src) = delete;                                    \
        TYPE(const TYPE& src) = default;                                                \
                                                                                        \
        virtual void MapReference(string identdata, void** member);                     \
                                                                                        \
    private:                                                                            \
        static BaseComponent* ComponentFactory(void* target_entity);                    \
                                                                                        \
    public:                                                                             \
        Uint32 GetType()                                                                \
        {                                                                               \
            return __ecs_factory_.GetType();                                            \
        }                                                                               \
        static Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType> __ecs_factory_

    /// Adds the component type to the registry by static instantiation and defines a virtual copy method.
    /// Add this to the class definition of a component that uses DECLARE_COMPONENT
    #define REGISTER_COMPONENT(TYPE)                                                                    \
    BaseComponent* TYPE::ComponentFactory(void* target_entity)                                          \
    {                                                                                                   \
        return ((Entity*)target_entity)->AddComponent<TYPE>();                                          \
    }                                                                                                   \
    void TYPE::MapReference(string identdata, void** member)                                            \
    {                                                                                                   \
        entity->MapReference(identdata, member);                                                        \
    }                                                                                                   \
    Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType> TYPE::__ecs_factory_(SID( #TYPE )::str, ComponentFactory);  \
                                                                                                        \
    TYPE* TYPE::Clone()                                                                                 \
    {                                                                                                   \
        return new TYPE(*this);                                                                         \
    }

    /// Constant return type id for a specified component type
    template<class T>
    const ComponentType GetComponentType()
    {
        return T::__ecs_factory_.GetType();
    }

    /// Dynamic type checking
    OSSIUM_EDL ComponentType GetComponentType(string name);
    OSSIUM_EDL string GetComponentName(ComponentType id);

    /// Forward declarations for the controller class
    class Entity;
    class BaseComponent;

    template <class T, class dummy = void>
    struct is_component
    {
        typedef BaseComponent type;

        constexpr bool operator()()
        {
            return value;
        }
        constexpr static bool value = false;
    };

    template <class T>
    struct is_component<T, typename enable_if<is_base_of<BaseComponent, T>::value, void>::type>
    {
        typedef T type;

        constexpr bool operator()()
        {
            return value;
        }
        constexpr static bool value = true;
    };

    /// Controls all entities and components at runtime and has access to engine services.
    class OSSIUM_EDL EntityComponentSystem
    {
    public:
        friend class Ossium::Entity;

        EntityComponentSystem(ServicesProvider* services);

        ~EntityComponentSystem();

        /// Iterates through all components that implement the Update() method and calls it for each one
        /// Note: as a minor optimisation, rather than calling this you could inherit from this class
        /// and replace with an override that only calls Update() on component types that use it.
        void UpdateComponents();

        /// SFINAE case, where type T is not a valid component type.
        template<typename T>
        typename enable_if<!is_component<T>::value, void>::type
        WalkComponents(function<void(T*)> operation)
        {
            Logger::EngineLog().Error("Attempted to walk over non-component type with ECS instance!");
        }

        /// Walks over all components of type T and operates on them.
        /// O(n) time complexity, where n == number of instances of component type T controlled by this ECS instance.
        template<typename T>
        typename enable_if<is_component<T>::value, void>::type
        WalkComponents(function<void(T*)> operation)
        {
            const ComponentType compType = GetComponentType<T>();
            for (unsigned int i = 0, counti = components[compType].empty() ? 0 : components[compType].size(); i < counti; i++)
            {
                for (auto itr : components[compType])
                {
                    operation(*itr);
                }
            }
        }

        /// Creates a new entity within this system and returns a reference to it.
        Entity* CreateEntity();

        /// Overload that accepts a parent entity reference, and if the parent exists within this system,
        /// the new entity will be added as a child of the parent.
        Entity* CreateEntity(Entity* parent);

        /// Destroys a single entity and all it's components. Logs a warning if the entity does not exist within this system.
        /// Note that the entity is destroyed at the end of a frame, prior to a RenderPresent call if 'immediate' is left false.
        /// Also note that you should only ever call this once on an entity, or you'll end up crashing (as you would with multiple `delete` calls).
        void DestroyEntity(Entity* entity, bool immediate = false);

        /// Actually deletes all entities pending destruction. This should only be called at the end of a frame once all components have been updated and/or rendered.
        void DestroyPending();

        /// Destroys ALL entities and their components
        void Clear();

        /// Returns the total number of entities
        unsigned int GetTotalEntities();

        /// Returns an array of all entities in the root of the hierarchy.
        vector<Entity*> GetRootEntities();

        /// Attempt to get an instance of a specific service type.
        template<typename T>
        T* GetService()
        {
            return servicesProvider->GetService<T>();
        }

        /// Serialise everything
        string ToString();
        void FromString(string& str);

    private:
        /// Removes the entity from the inactiveEntities set.
        void SetActive(Entity* entity);

        /// Inserts the entity into the inactiveEntities set.
        void SetInactive(Entity* entity);

        /// Returns false if the entity is in the inactiveEntities set.
        bool IsActive(Entity* entity);

        /// All GLOBALLY inactive entities - includes entities that could be locally active.
        unordered_set<Entity*> inactiveEntities;

        /// Vector of pointers to ALL component instances, inside an array ordered by component type.
        /// This is maintained because it's more efficient when updating or rendering lots of components
        /// of a specific type each frame
        vector<BaseComponent*>* components;

        /// All entities currently pending destruction. These will be destroyed at the end of the frame.
        /// They cannot be removed once added until they are destroyed.
        vector<Entity*> pendingDestruction;

        /// Entity tree hierarchy (pure scene graph).
        Tree<Entity*> entityTree;

        /// Hash table of entity nodes by id
        unordered_map<int, Node<Entity*>*> entities;

        /// Direct map of ids to reference type members that point to entities or components
        unordered_map<string, set<void**>> serialised_pointers;

        /// Provider for non-static engine services such as a ResourceController instance.
        ServicesProvider* servicesProvider = nullptr;

    };

    class OSSIUM_EDL Entity : public SchemaReferable
    {
    public:
        friend class EntityComponentSystem;

        /// Instantiates and attaches a component to this entity.
        template<class T>
        T* AddComponent()
        {
            T* component = new T();
            component->entity = this;
            component->OnCreate();
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end())
            {
                itr->second.push_back(component);
            }
            else
            {
                vector<BaseComponent*> component_vector;
                component_vector.push_back(component);
                components.insert({GetComponentType<T>(), component_vector});
            }
            /// Add the component to the ECS controller
            controller->components[GetComponentType<T>()].push_back(component);
            return component;
        }

        /// Adds a component if it hasn't been added already.
        template<class T>
        T* AddComponentOnce()
        {
            T* component = GetComponent<T>();
            if (component == nullptr)
            {
                component = AddComponent<T>();
            }
            return component;
        }

        /// Destroys and removes first found instance of a component attached to this entity
        template<class T>
        void RemoveComponent()
        {
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end() && !itr->second.empty() && itr->second[0] != nullptr)
            {
                vector<BaseComponent*>& ecs_components = controller->components[GetComponentType<T>()];
                /// First, remove the component pointer from the EntityComponentSystem
                for (auto i = ecs_components.begin(); i != ecs_components.end(); i++)
                {
                    if (*i == itr->second[0])
                    {
                        ecs_components.erase(i);
                        break;
                    }
                }
                /// Now remove the component pointer from this entity's components hash and delete it
                itr->second[0]->OnDestroy();
                delete itr->second[0];
                itr->second[0] = nullptr;
                itr->second.erase(itr->second.begin());
            }
            Logger::EngineLog().Warning("(!) Could not find any component of type[{0}] attached to entity[{1}] with name '{2}'.", GetComponentType<T>(), self->id, name);
        }

        /// Returns a pointer the first found instance of a component attached
        /// to this entity of type T
        template <class T>
        T* GetComponent()
        {
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end() && !itr->second.empty())
            {
                return static_cast<T*>(itr->second[0]);
            }
            return nullptr;
        }

        /// Returns a vector of pointers to all component instances of a given type
        /// attached to this entity
        template <class T>
        vector<T*> GetComponents()
        {
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end())
            {
                vector<T*> retComponents;
                for (auto i = itr->second.begin(); i != itr->second.end(); i++)
                {
                    retComponents.push_back(static_cast<T*>(*i));
                }
                return retComponents;
            }
            // Return an empty vector
            vector<T*> none;
            return none;
        }

        /// Returns a reference to the Component* array of a specified type.
        /// Faster than the GetComponents() template but doesn't do any type conversion.
        vector<BaseComponent*>& GetComponents(ComponentType compType);

        template <class T>
        bool HasComponent()
        {
            return GetComponent<T>() != nullptr;
        }

        /// Returns first instance of a given component in this entity's children
        template<class T>
        T* GetComponentInChildren()
        {
            for (unsigned int i = 0, counti = self->children.empty() ? 0 : self->children.size(); i < counti; i++)
            {
                T* component = self->children[i]->data->GetComponent<T>();
                if (component != nullptr)
                {
                    return component;
                }
            }
            return nullptr;
        }

        /// Returns ALL matching components in this entity's children
        template<class T>
        vector<T*> GetComponentsInChildren()
        {
            vector<T*> output;
            for (unsigned int i = 0, counti = self->children.empty() ? 0 : self->children.size(); i < counti; i++)
            {
                vector<T*> data = self->children[i]->data->GetComponents<T>();
                for (unsigned int j = 0, countj = self->children.empty() ? 0 : self->children.size(); j < countj; j++)
                {
                    output.push_back(data[j]);
                }
            }
            return output;
        }

        /// Attempt to get an instance of a specific service type.
        template<typename T>
        T* GetService()
        {
            return controller->GetService<T>();
        }

        /// Maps an id to a reference for serialisation.
        void MapReference(string ident, void** ptr);

        Entity* GetParent();
        void SetParent(Entity* parent);

        /// Returns this entity's ID
        const int GetID();

        /// Ditto but in string form, required by SchemaReferable derived types.
        string GetReferenceID();

        /// Returns true when this entity is active in the scene (i.e. no parents are inactive
        /// and not inactive locally).
        bool IsActive();
        /// Returns true when this entity is active locally, regardless of whether it is active in the scene or not.
        bool IsActiveLocally();

        /// Sets the local state of this entity to active or inactive.
        void SetActive(bool activate);

        /// This effectively replaces the copy constructor; entities can only be explicitly copied
        Entity* Clone();

        /// Returns pointer to first found instance of an entity
        Entity* Find(string name);
        /// Ditto, but searches only for entities below the parent
        Entity* Find(string name, Entity* parent);

        /// Creates a child entity of this entity
        Entity* CreateChild();

        /// Returns a reference to the ECS instance.
        EntityComponentSystem* GetECS();

        /// String conversion methods get/set with the JSON representation of all attached components.
        void FromString(string& str);
        string ToString();

        /// The name of this entity.
        string name;

        void Destroy(bool immediate = false);

    private:
        /// Direct creation of entities is not permitted; you can only create new entities via the Clone() method,
        /// or by calling CreateEntity() on an EntityComponentSystem instance
        Entity(EntityComponentSystem* entity_system, Entity* parent = nullptr);
        ~Entity();

        /// Direct copying of entities is not permitted! Use Clone() if a copy is necessary
        Entity(Entity& copySource) = delete;
        Entity(const Entity& copySource) = delete;
        Entity& operator=(const Entity& source) = delete;

        /// Setup active state etc.
        void OnSceneLoaded();

        /// Set active in the scene
        void SetActiveInScene();
        /// Set inactive in the scene
        void SetInactiveInScene();

        /// Hashtable of components attached to this entity by type
        unordered_map<ComponentType, vector<BaseComponent*>> components;

        /// Pointer to the system this entity exists in
        EntityComponentSystem* controller;

        /// Pointer to the node containing this entity
        Node<Entity*>* self;

        /// Is this entity active (locally) in the scene?
        bool active = true;

    };

    struct OSSIUM_EDL ComponentSchema : public Schema<ComponentSchema, 1>
    {
        DECLARE_BASE_SCHEMA(ComponentSchema, 1);

        ///
        /// Warning: extend max members before adding new members!
        ///
        M(bool, enabled) = true;

    };

    /// Base class for all components
    /// WARNING: INHERITANCE ORDER MUST NOT BE CHANGED as it affects the STRUCTURAL LAYOUT which schemas are dependant upon.
    class OSSIUM_EDL BaseComponent : public SchemaReferable, public ComponentSchema
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, ComponentSchema);

        friend class Entity;
        friend class EntityComponentSystem;

        /// Returns a pointer to the entity this component is attached to.
        Entity* GetEntity();

        virtual Uint32 GetType() = 0;

        string GetReferenceID();

        /// Returns true only when enabled and the associated entity is active.
        bool IsActiveAndEnabled();

    protected:
        /// These replace the constructor and destructor.
        virtual void OnCreate();
        virtual void OnDestroy();

        /// Called just before the component is serialised.
        virtual void OnLoadStart();
        /// Called once the entire related Entity Component System has been serialised.
        virtual void OnLoadFinish();

        /// Called when this component is copied; replaces the copy constructor
        virtual void OnClone(BaseComponent* src);

        /// Each frame this method is called
        virtual void Update();

        /// Attempt to get an instance of a specific service type.
        template<typename T>
        T* GetService()
        {
            return entity->GetService<T>();
        }

        /// Pointer to the entity that this component is attached to
        Entity* entity = nullptr;

        /// A cloning method is required for polymorphic copies, e.g. when copying an entity
        /// we need to perform a deep copy of different component types in a vector<Component*>.
        /// This is implemented automagically by the REGISTER_COMPONENT(TYPE) macro.
        virtual BaseComponent* Clone() = 0;

        virtual ~BaseComponent();

        /// Only friend class Entity can instantiate components
        BaseComponent();

        /// Copying of components by the base copy constructor isn't allowed, use Clone() instead
        BaseComponent(const BaseComponent& copySource);
        BaseComponent& operator=(const BaseComponent& copySource) = delete;

    };

    #define DECLARE_ABSTRACT_COMPONENT(TYPE)                                    \
            TYPE();                                                             \
            virtual ~TYPE();                                                    \
                                                                                \
            virtual void OnCreate();                                            \
            virtual void OnDestroy();                                           \
                                                                                \
            virtual void OnLoadStart();                                         \
            virtual void OnLoadFinish();                                        \
                                                                                \
            virtual void OnClone(BaseComponent* src);                           \
                                                                                \
            virtual void Update();                                              \
                                                                                \
            virtual TYPE* Clone() = 0;                                          \
                                                                                \
            virtual Uint32 GetType() = 0;

    #define REGISTER_ABSTRACT_COMPONENT(TYPE, BASETYPE)                 \
        TYPE::TYPE() {}                                                 \
        TYPE::~TYPE() {}                                                \
        void TYPE::OnCreate() { BASETYPE::OnCreate(); }                 \
        void TYPE::OnDestroy() { BASETYPE::OnDestroy(); }               \
        void TYPE::OnLoadStart() { BASETYPE::OnLoadStart(); }           \
        void TYPE::OnLoadFinish() { BASETYPE::OnLoadFinish(); }         \
        void TYPE::OnClone(BaseComponent* src) {}                       \
        void TYPE::Update(){}

}

#endif // ECS_H
