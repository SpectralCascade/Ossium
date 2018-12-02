#pragma once

#ifndef ECS_H
#define ECS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "transform.h"

using namespace std;

namespace ossium
{

    typedef Uint32 ComponentType;

    namespace ecs
    {
        /// This is a compile-time construct designed to provide compile-time type identification
        /// as an efficient alternative to RTTI
        class ComponentRegistry
        {
        private:
            static ComponentType nextTypeIdent;
            /// The type identifier for this registered component
            ComponentType typeIdent;

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

        };
    }

    /// Declares a component type and declares a virtual copy method
    /// Add this to the end of any class you wish to register as a component
    #define DECLARE_COMPONENT(TYPE) public: static ecs::ComponentRegistry __ecs_entry_; \
                                                                                        \
    TYPE* Clone()
    /// Adds the component type to the registry by static instantiation and defines a virtual copy method
    /// Add this to the class definition file of a component that uses DECLARE_COMPONENT
    #define REGISTER_COMPONENT(TYPE) ecs::ComponentRegistry TYPE::__ecs_entry_;         \
                                                                                        \
    TYPE* TYPE::Clone()                                                           \
    {                                                                                   \
        return new TYPE(*this);                                                         \
    }

    /// Compile time constant return type
    template<class T>
    ComponentType getComponentType()
    {
        return T::__ecs_entry_.getType();
    }

    class Component;

    /// Nothing should inherit from Entity.
    /// I would use the 'final' keyword to ensure this
    /// but Code::Blocks autocomplete stops working properly
    class Entity
    {
    public:
        Entity();
        /// Copying entities is extremely useful, but only if performing a deep copy!
        Entity(const Entity& copySource);
        Entity& operator=(Entity copySource);

        void Swap(Entity& itemOne, Entity& itemTwo);

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
        }

        /// Destroys and removes first found instance of a component attached to this entity
        template<class T>
        void RemoveComponent()
        {
            auto itr = components.find(getComponentType<T>());
            if (itr != components.end() && !itr->second.empty())
            {
                itr->second[0]->OnDestroy();
                delete itr->second[0];
                itr->second[0] = nullptr;
                itr->second.erase(itr->second.begin());
            }
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

        /// Returns this entity's ID
        const int GetID();

        /// Name of this entity (not necessarily unique)
        string name;

        /// Transform data for this entity
        Transform transform;

    private:
        /// Hashtable of components attached to this entity by type
        unordered_map<ComponentType, vector<Component*>> components;

        /// The ID of this entity (unique)
        int id;

        /// Incremental id
        static int nextId;

    };

    /// Base class for all components
    class Component
    {
    public:
        friend class Entity;

        /// Initialise entity pointer to null
        Component();

        /// A cloning method is required for polymorphic copies, e.g. when copying an entity
        /// we need to perform a deep copy of the components in a vector<Component*>, not the base Component instance
        virtual Component* Clone() = 0;

        /// Make sure derived classes are destroyed properly
        virtual ~Component();

        /// Returns a reference to the entity this component is attached to
        Entity& GetEntity();

    protected:
        /// Effectively replace the constructor and destructor
        virtual void OnCreate();
        virtual void OnDestroy();

        /// Called when the parent entity is spawned
        virtual void OnSpawn();

        /// Each frame this method is called
        virtual void Update();

    private:
        /// Copying of components by the base copy constructor isn't allowed, use Clone() instead
        Component(const Component& copySource);
        Component& operator=(const Component& copySource);

        /// Pointer to the entity that this component is attached to
        /// Should never be null, assuming
        Entity* entity;

        #ifdef DEBUG
        bool onDestroyCalled;
        #endif // DEBUG

    };

}

#endif // ECS_H
