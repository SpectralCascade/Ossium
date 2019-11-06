#pragma once

#ifndef ECS_H
#define ECS_H

#include <vector>
#include <string.h>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <SDL.h>

#include "renderer.h"
#include "tree.h"
#include "stringintern.h"
#include "schemamodel.h"

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
        static StrID __component_type;                                                  \
                                                                                        \
        static Component* ComponentFactory(void* target_entity);                        \
                                                                                        \
    public:                                                                             \
        Uint32 GetType()                                                                \
        {                                                                               \
            return __ecs_factory_.GetType();                                            \
        }                                                                               \
        static Ossium::typesys::TypeFactory<Component, ComponentType> __ecs_factory_

    /// Adds the component type to the registry by static instantiation and defines a virtual copy method.
    /// Add this to the class definition of a component that uses DECLARE_COMPONENT
    #define REGISTER_COMPONENT(TYPE)                                                                    \
    Component* TYPE::ComponentFactory(void* target_entity)                                              \
    {                                                                                                   \
        return ((Entity*)target_entity)->AddComponent<TYPE>();                                          \
    }                                                                                                   \
    void TYPE::MapReference(string identdata, void** member)                                            \
    {                                                                                                   \
        entity->MapReference(identdata, member);                                                        \
    }                                                                                                   \
    Ossium::typesys::TypeFactory<Component, ComponentType> TYPE::__ecs_factory_(SID( #TYPE )::str, ComponentFactory);  \
    StrID TYPE::__component_type = SID(#TYPE)::str;                                                     \
                                                                                                        \
    TYPE* TYPE::Clone()                                                                                 \
    {                                                                                                   \
        return new TYPE(*this);                                                                         \
    }

    /// Constant return type id for a specified component type
    template<class T>
    ComponentType GetComponentType()
    {
        return T::__ecs_factory_.GetType();
    }

    /// Dynamic type checking
    ComponentType GetComponentType(string name);
    string GetComponentName(ComponentType id);

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

        /// Serialise everything
        string ToString();
        void FromString(string& str);

    private:
        /// Vector of pointers to ALL component instances, inside an array ordered by component type.
        /// This is maintained because it's more efficient when updating or rendering lots of components
        /// of a specific type each frame
        vector<Component*>* components;

        /// All entities currently pending destruction. These will be destroyed at the end of the frame.
        /// They cannot be removed once added until they are destroyed.
        vector<Entity*> pendingDestruction;

        /// Entity tree hierarchy
        Tree<Entity*> entityTree;

        /// Hash table of entity nodes by id
        unordered_map<int, Node<Entity*>*> entities;

        /// Direct map of ids to reference type members that point to entities or components
        unordered_map<string, set<void**>> serialised_pointers;

    };

    class Entity
    {
    public:
        friend class EntityComponentSystem;

        /// Instantiates and attaches a component to this entity
        template<class T>
        T* AddComponent(Renderer* renderer = nullptr, int layer = -1)
        {
            T* component = new T();
            component->entity = this;
            component->OnCreate();
            component->OnInitGraphics(renderer, layer);
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end())
            {
                itr->second.push_back(component);
            }
            else
            {
                vector<Component*> component_vector;
                component_vector.push_back(component);
                components.insert({GetComponentType<T>(), component_vector});
            }
            /// Add the component to the ECS controller
            controller->components[GetComponentType<T>()].push_back(component);
            return component;
        }

        /// Destroys and removes first found instance of a component attached to this entity
        template<class T>
        void RemoveComponent()
        {
            auto itr = components.find(GetComponentType<T>());
            if (itr != components.end() && !itr->second.empty() && itr->second[0] != nullptr)
            {
                vector<Component*>& ecs_components = controller->components[GetComponentType<T>()];
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
                itr->second[0]->OnRemoveGraphics();
                itr->second[0]->OnDestroy();
                delete itr->second[0];
                itr->second[0] = nullptr;
                itr->second.erase(itr->second.begin());
            }
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "(!) Could not find any component of type[%d] attached to entity[%d] with name '%s'.", GetComponentType<T>(), self->id, self->name.c_str());
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
        vector<Component*>& GetComponents(ComponentType compType);

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

        /// Maps an id to a reference for serialisation.
        void MapReference(string ident, void** ptr);

        Entity* GetParent();
        void SetParent(Entity* parent);

        /// Returns this entity's ID
        const int GetID();

        /// This effectively replaces the copy constructor; entities can only be explicitly copied
        Entity* Clone();

        /// Returns pointer to first found instance of an entity
        Entity* Find(string name);
        /// Ditto, but searches only for entities below the parent
        Entity* Find(string name, Entity* parent);

        /// Creates a child entity of this entity
        Entity* CreateChild();

        /// String conversion methods get/set with the JSON representation of all attached components.
        void FromString(string& str);
        string ToString();

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

        /// Hashtable of components attached to this entity by type
        unordered_map<ComponentType, vector<Component*>> components;

        /// Pointer to the system this entity exists in
        EntityComponentSystem* controller;

        /// Pointer to the node containing this entity
        Node<Entity*>* self;

    };

    struct ComponentSchema : public Schema<ComponentSchema, 0>
    {
        DECLARE_BASE_SCHEMA(ComponentSchema, 0);

        ///
        /// Warning: extend max members before adding new members!
        ///

    };

    /// Base class for all components
    class Component : public ComponentSchema
    {
    public:
        friend class Entity;
        friend class EntityComponentSystem;

        CONSTRUCT_SCHEMA(SchemaRoot, ComponentSchema);

        /// Returns a pointer to the entity this component is attached to.
        Entity* GetEntity();

        virtual Uint32 GetType() = 0;

    protected:
        /// These replace the constructor and destructor
        virtual void OnCreate();
        virtual void OnDestroy();

        /// This follows up the OnCreate() call, allowing a component to initialise and register graphics with the provided renderer.
        virtual void OnInitGraphics(Renderer* renderer, int layer = -1);
        /// This is called just before the OnDestroy() method is called. Use this to unregister graphics from the renderer if necessary.
        virtual void OnRemoveGraphics();

        /// Called when this component is copied; replaces the copy constructor
        virtual void OnClone();

        /// Each frame this method is called
        virtual void Update();

        /// Pointer to the entity that this component is attached to
        Entity* entity = nullptr;

        /// A cloning method is required for polymorphic copies, e.g. when copying an entity
        /// we need to perform a deep copy of different component types in a vector<Component*>.
        /// This is implemented automagically by the REGISTER_COMPONENT(TYPE) macro.
        virtual Component* Clone() = 0;

        virtual ~Component();

        /// Only friend class Entity can instantiate components
        Component();

        /// Copying of components by the base copy constructor isn't allowed, use Clone() instead
        Component(const Component& copySource);
        Component& operator=(const Component& copySource) = delete;

    };

    template <class T, class dummy = void>
    struct is_component
    {
        typedef Component type;

        constexpr bool operator()()
        {
            return value;
        }
        constexpr static bool value = false;
    };

    template <class T>
    struct is_component<T, typename enable_if<is_base_of<Component, T>::value, void>::type>
    {
        typedef T type;

        constexpr bool operator()()
        {
            return value;
        }
        constexpr static bool value = true;
    };

    #define DECLARE_ABSTRACT_COMPONENT(TYPE)                                    \
            TYPE();                                                             \
            virtual ~TYPE();                                                    \
                                                                                \
            virtual void OnCreate();                                            \
            virtual void OnDestroy();                                           \
                                                                                \
            virtual void OnInitGraphics(Renderer* renderer, int layer = -1);    \
                                                                                \
            virtual void OnRemoveGraphics();                                    \
                                                                                \
            virtual void OnClone();                                             \
                                                                                \
            virtual void Update();                                              \
                                                                                \
            virtual TYPE* Clone() = 0;                                          \
                                                                                \
            virtual Uint32 GetType() = 0;

    #define REGISTER_ABSTRACT_COMPONENT(TYPE)                           \
        TYPE::TYPE() {}                                                 \
        TYPE::~TYPE() {}                                                \
        void TYPE::OnCreate() {}                                        \
        void TYPE::OnDestroy() {}                                       \
        void TYPE::OnClone() {}                                         \
        void TYPE::Update(){}                                           \
        void TYPE::OnInitGraphics(Renderer* renderer, int layer){}      \
        void TYPE::OnRemoveGraphics(){}

    #define DECLARE_ABSTRACT_GRAPHIC_COMPONENT(TYPE)                    \
        DECLARE_ABSTRACT_COMPONENT(TYPE)

    #define REGISTER_ABSTRACT_GRAPHIC_COMPONENT(TYPE)                   \
        TYPE::TYPE() {}                                                 \
        TYPE::~TYPE() {}                                                \
        void TYPE::OnCreate() {}                                        \
        void TYPE::OnDestroy() {}                                       \
        void TYPE::OnClone() {}                                         \
        void TYPE::Update(){}                                           \
        void TYPE::OnInitGraphics(Renderer* renderer, int layer)        \
        {                                                               \
            GraphicComponent::OnInitGraphics(renderer, layer);          \
        }                                                               \
        void TYPE::OnRemoveGraphics()                                   \
        {                                                               \
            GraphicComponent::OnRemoveGraphics();                       \
        }

    inline namespace Graphics
    {

        /// A type of Component automatically registers and unregisters itself from a renderer instance upon creation.
        /// Also holds on to a reference to the renderer in use.
        class GraphicComponent : public Graphic, public Component
        {
        public:
            /// Attempts to set the rendering layer of this graphic component. Note that you probably shouldn't call this
            /// too frequently as it attempts a removal from one set and insertion into another set within the renderer.
            void SetRenderLayer(int layer);

            /// Returns the layer this component is being rendered on.
            int GetRenderLayer();

        protected:
            DECLARE_ABSTRACT_COMPONENT(GraphicComponent);

            virtual void Render(Renderer& renderer) = 0;

            /// Pointer to the renderer instance this graphic component is registered to.
            Renderer* rendererInstance = nullptr;

        private:
            /// The rendering layer this graphic component should use.
            int renderLayer = 0;

        };

    }

}

#endif // ECS_H
