#pragma once

#ifndef ECS_H
#define ECS_H

#include <vector>
#include <string.h>
#include <unordered_map>
#include <SDL.h>

#include "transform.h"
#include "tree.h"

using namespace std;

namespace Ossium
{

    typedef Uint32 ComponentType;

    namespace ecs
    {
        /// This is a class designed to provide custom type identification
        /// as an efficient alternative to RTTI.
        class ComponentRegistry
        {
        private:
            static ComponentType nextTypeIdent;
            /// The type identifier for this registered component
            ComponentType typeIdent;

            /// Whether or not the registered component type implements the Update() method
            bool update;

        public:
            ComponentRegistry()
            {
                typeIdent = nextTypeIdent;
                /// Now increment nextTypeIdent for when we register another type
                nextTypeIdent++;
            }

            /// The type identifier never changes once set for a type, hence const
            const ComponentType getType()
            {
                return typeIdent;
            }

            static Uint32 GetTotalTypes()
            {
                return (Uint32)nextTypeIdent;
            }

        };

    }

    /// Declares a component type and declares a virtual copy method
    /// Add this to the end of any class you wish to register as a component
    #define DECLARE_COMPONENT(TYPE) public: static ecs::ComponentRegistry __ecs_entry_; \
                                                                                        \
    TYPE* Clone()
    /// Adds the component type to the registry by static instantiation and defines a virtual copy method
    /// Add this to the class definition of a component that uses DECLARE_COMPONENT
    #define REGISTER_COMPONENT(TYPE) ecs::ComponentRegistry TYPE::__ecs_entry_;         \
                                                                                        \
    TYPE* TYPE::Clone()                                                                 \
    {                                                                                   \
        return new TYPE(*this);                                                         \
    }

    /// Constant return type id for a specified component type
    template<class T>
    ComponentType getComponentType()
    {
        return T::__ecs_entry_.getType();
    }

    /// Forward declarations for the controller class
    class Entity;
    class Component;

    /// Controls all entities and components at runtime
    class EntityComponentSystem
    {
    public:
        friend class Ossium::Entity;

        EntityComponentSystem();
        ~EntityComponentSystem();

        /// Iterates through all components that implement the Update() method and calls it for each one
        void UpdateComponents();

        /// Removes ALL entities and their components
        void Clear();

        /// Returns the total number of entities
        unsigned int GetTotalEntities();

    private:
        /// Vector of pointers to ALL component instances, inside an array ordered by component type.
        /// This is maintained because it's more efficient when updating or rendering lots of components
        /// of a specific type each frame
        vector<Component*>* components;

        /// Entity tree hierarchy
        Tree<Entity*> entityTree;

        /// Hash table of entity nodes by id
        unordered_map<int, Node<Entity*>*> entities;

    };

    class Entity final
    {
    public:
        friend class EntityComponentSystem;

        Entity(EntityComponentSystem* entity_system, Entity* parent = nullptr);
        ~Entity();

        /// Instantiates and attaches a component to this entity
        template<class T>
        void AttachComponent()
        {
            T* component = new T();
            component->entity = this;
            component->OnCreate();
            auto itr = components.find(getComponentType<T>());
            if (itr != components.end())
            {
                itr->second.push_back(component);
            }
            else
            {
                vector<Component*> component_vector;
                component_vector.push_back(component);
                components.insert({getComponentType<T>(), component_vector});
            }
            /// Add the component to the ECS controller
            controller->components[getComponentType<T>()].push_back(component);
        }

        /// Destroys and removes first found instance of a component attached to this entity
        template<class T>
        void RemoveComponent()
        {
            auto itr = components.find(getComponentType<T>());
            if (itr != components.end() && !itr->second.empty() && itr->second[0] != nullptr)
            {
                vector<Component*>& ecs_components = controller->components[getComponentType<T>()];
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
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "(!) Could not find any component of type[%d] attached to entity[%d] with name '%s'.", getComponentType<T>(), self->id, self->name.c_str());
        }

        /// Returns a pointer the first found instance of a component attached
        /// to this entity of type T
        template <class T>
        T* GetComponent()
        {
            auto itr = components.find(getComponentType<T>());
            if (itr != components.end() && !itr->second.empty())
            {
                return reinterpret_cast<T*>(itr->second[0]);
            }
            return nullptr;
        }

        /// Returns a vector of pointers to all component instances of a given type
        /// attached to this entity
        template <class T>
        vector<T*> GetComponents()
        {
            auto itr = components.find(getComponentType<T>());
            if (itr != components.end())
            {
                vector<T*> retComponents;
                for (auto i = itr->second.begin(); i != itr->second.end(); i++)
                {
                    retComponents.push_back(reinterpret_cast<T*>(*i));
                }
                return retComponents;
            }
            // Return an empty vector
            vector<T*> none;
            return none;
        }

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

        string GetName();
        void SetName(string name);

        Entity* GetParent();

        /// Returns this entity's ID
        const int GetID();

        /// Transform data for this entity
        Transform transform;

        /// This effectively replaces the copy constructor; entities can only be explicitly copied
        Entity* Clone();

        /// Returns pointer to first found instance of an entity
        Entity* find(string name);
        /// Ditto, but searches only for entities below the parent
        Entity* find(string name, Entity* parent);

    private:
        /// Direct copying of entities is not permitted! Use Clone() if a copy is necessary
        Entity(Entity& copySource);
        Entity(const Entity& copySource);
        Entity& operator=(const Entity& source);

        /// Hashtable of components attached to this entity by type
        unordered_map<ComponentType, vector<Component*>> components;

        /// Pointer to the system this entity exists in
        EntityComponentSystem* controller;

        /// Pointer to the node containing this entity
        Node<Entity*>* self;

    };

    /// Base class for all components
    class Component
    {
    public:
        friend class Entity;
        friend class EntityComponentSystem;

        /// Returns a pointer to the entity this component is attached to
        Entity* GetEntity();

    protected:
        /// Effectively replace the constructor and destructor
        virtual void OnCreate();
        virtual void OnDestroy();

        /// Called when the parent entity is spawned
        virtual void OnSpawn();

        /// Each frame this method is called
        virtual void Update();

        /// Pointer to the entity that this component is attached to
        Entity* entity;

        /// Initialise entity pointer to null
        /// Protected - only friend class Entity can instantiate components
        Component();

        /// Make sure derived classes are destroyed properly
        virtual ~Component();

    private:
        /// A cloning method is required for polymorphic copies, e.g. when copying an entity
        /// we need to perform a deep copy of different component types in a vector<Component*>
        /// This is implemented automagically by the DECLARE_COMPONENT(TYPE) and REGISTER_COMPONENT(TYPE) macros
        virtual Component* Clone() = 0;

        /// Copying of components by the base copy constructor isn't allowed, use Clone() instead
        Component(const Component& copySource);
        Component& operator=(const Component& copySource);

        #ifdef DEBUG
        bool onDestroyCalled;
        #endif // DEBUG

    };

}

#endif // ECS_H
