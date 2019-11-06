#ifndef RESOURCECONTROLLER_H
#define RESOURCECONTROLLER_H

#include <string.h>
#include <unordered_map>
#include <SDL.h>

#include "basics.h"
#include "renderer.h"

using namespace std;

namespace Ossium
{

    typedef Uint32 ResourceType;

    /// Forward declaration for Resource class
    class ResourceController;

    #define DECLARE_RESOURCE(TYPE)                                                                                      \
    friend class ResourceController;                                                                                    \
    private:                                                                                                            \
        static Resource* ResourceFactory(void* target_controller);                                                      \
    public:                                                                                                             \
        static Ossium::typesys::TypeFactory<Resource, ResourceType> __resource_factory_

    #define REGISTER_RESOURCE(TYPE)                                                                                     \
    Resource* TYPE::ResourceFactory(void* target_controller)                                                            \
    {                                                                                                                   \
        return ((ResourceController*)target_controller)->Load<TYPE>("");                                                \
    }                                                                                                                   \
    Ossium::typesys::TypeFactory<Resource, ResourceType> TYPE::__resource_factory_(SID( #TYPE )::str, ResourceFactory);

    /// All resource classes e.g. images, audio clips etc. should inherit from this base class
    class Resource
    {
    public:
        Resource() = default;
        virtual ~Resource() = 0;

        virtual bool Load(string filePath) = 0;
        virtual bool Init(string args = "");

    protected:
        ResourceController* controller;

    };

    /// Resource controller that deals with resources of various types e.g. images, audio clips.
    class ResourceController
    {
    public:
        ResourceController();

        /// Attempts to load a resource
        template<class T>
        T* Load(string guid_path)
        {
            bool success = true;
            T* resource = new T();
            if (!resource->Load(guid_path))
            {
                success = false;
            }
            if (success)
            {
                /// Add to registry
                registry<T>()[guid_path] = resource;
            }
            else
            {
                delete resource;
                resource = nullptr;
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to load resource '%s'.", guid_path.c_str());
            }
            return resource;
        };

        /// Post-load initialisation method for resources.
        template<class T>
        bool Init(string guid_path)
        {
            T* resource = Find<T>(guid_path);
            if (resource != nullptr)
            {
                if (resource->Init())
                {
                    return true;
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Cannot find resource '%s' for post-load initialisation!", guid_path.c_str());
            }
            return false;
        };

        /// Destroys a resource and removes it from the registry
        template<class T>
        void Free(string guid_path)
        {
            if (registry<T>().find(guid_path) != registry<T>().end())
            {
                if (registry<T>()[guid_path] != nullptr)
                {
                    delete registry<T>()[guid_path];
                }
                registry<T>().erase(guid_path);
            }
        };

        /// Returns pointer to a resource, or nullptr if the GUID doesn't exist in the registry
        template<class T>
        T* Find(string guid_path)
        {
            if (registry<T>().find(guid_path) == registry<T>().end())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to retrieve resource with GUID '%s'!", guid_path.c_str());
                return nullptr;
            }
            return reinterpret_cast<T*>(registry<T>()[guid_path]);
        };

        /// Destroys all resources of all types
        void FreeAll()
        {
            /// Iterate through registries of all types and delete everything
            for (ResourceType rtype = 0; rtype < typesys::TypeRegistry<Resource>::GetTotalTypes(); rtype++)
            {
                auto& reg = registryByType[rtype];
                for (auto i = reg.begin(); i != reg.end(); i++)
                {
                    if (i->second != nullptr)
                    {
                        delete i->second;
                    }
                }
                reg.clear();
            }
        };

        /// Destroys all resources of a specific type
        template<class T>
        void FreeAll()
        {
            for (auto i = registry<T>().begin(); i != registry<T>().end(); i++)
            {
                if (i->second != nullptr)
                {
                    delete i->second;
                }
            }
            registry<T>().clear();
        };

    private:
        /// We definitely don't want to shallow copy resources
        NOCOPY(ResourceController);

        /// Returns a reference to the registry for a particular type.
        template<class T>
        unordered_map<string, Resource*>& registry()
        {
            return *registryByType[T::__resource_factory::GetType()];
        }

        /// Lookup registry array, ordered by type id; key = guid_path, value = pointer to resource
        unordered_map<string, Resource*>* registryByType;

    };

}

#endif // RESOURCECONTROLLER_H
