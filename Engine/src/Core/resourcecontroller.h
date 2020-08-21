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
#ifndef RESOURCECONTROLLER_H
#define RESOURCECONTROLLER_H

#include <string.h>
#include <unordered_map>

#include "stringconvert.h"
#include "stringintern.h"
#include "services.h"
#include "resource.h"

namespace Ossium
{

    /// Resource controller that deals with resources of various types e.g. images, audio clips.
    class OSSIUM_EDL ResourceController : public Service<ResourceController>
    {
    public:
        ResourceController();

        /// Attempts to load a resource
        template<typename T, typename ...Args>
        T* Load(std::string guid_path, Args&&... args)
        {
            bool success = true;
            T* resource = Find<T>(guid_path);
            if (resource == nullptr)
            {
                resource = new T();
            }
            if (!resource->Load(guid_path, std::forward<Args>(args)...))
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
                Log.Warning("Failed to load resource '{0}.", guid_path);
            }
            return resource;
        };

        /// Attempts to initialise a resource post load.
        template<typename T, typename ...Args>
        T* Init(std::string guid_path, Args&&... args)
        {
            T* resource = Find<T>(guid_path);
            if (resource != nullptr)
            {
                resource->Init(std::forward<Args>(args)...);
            }
            else
            {
                Log.Warning("Cannot find resource '{0}' for post-load initialisation!", guid_path);
            }
            return resource;
        }

        /// Attempts to load and initialise a resource.
        template<typename T, typename ...Args>
        T* LoadAndInit(std::string guid_path, Args&&... args)
        {
            T* resource = Find<T>(guid_path);
            if (resource == nullptr)
            {
                resource = new T();
            }
            if (resource != nullptr)
            {
                if (resource->LoadAndInit(guid_path, std::forward<Args>(args)...))
                {
                    /// Add to registry if it isn't already added.
                    registry<T>()[guid_path] = resource;
                }
                else
                {
                    delete resource;
                    resource = nullptr;
                    Log.Warning("Failed to load resource '{0}'.", guid_path);
                }
            }
            else
            {
                Log.Warning("Cannot find resource '{0}' for post-load initialisation!", guid_path);
            }
            return resource;
        }

        /// Returns a resource, or attempts to load and initialise a resource if it does not exist.
        template<typename T, typename ...Args>
        T* Get(std::string guid_path, Args&&... args)
        {
            T* resource = Find<T>(guid_path);
            if (resource == nullptr)
            {
                resource = LoadAndInit<T>(guid_path, std::forward<Args>(args)...);
            }
            return resource;
        }

        /// Destroys a resource and removes it from the registry
        template<class T>
        void Free(std::string guid_path)
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
        T* Find(std::string guid_path)
        {
            auto found = registry<T>().find(guid_path);
            if (found == registry<T>().end())
            {
                //Log.Warning("Failed to retrieve resource with GUID '{0}'!", guid_path);
                return nullptr;
            }
            return reinterpret_cast<T*>(found->second);
        };

        /// Returns the map of all loaded resources of the specified type.
        template<typename T>
        std::unordered_map<std::string, Resource*>& GetAll()
        {
            return registry<T>();
        }

        /// Destroys all resources of all types
        void FreeAll()
        {
            /// Iterate through registries of all types and delete everything
            for (ResourceType rtype = 0; rtype < TypeSystem::TypeRegistry<Resource>::GetTotalTypes(); rtype++)
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
        std::unordered_map<std::string, Resource*>& registry()
        {
            return registryByType[T::__resource_factory.GetType()];
        }

        /// Lookup registry array, ordered by type id; key = guid_path, value = pointer to resource
        std::unordered_map<std::string, Resource*>* registryByType;

    };

}

#endif // RESOURCECONTROLLER_H
