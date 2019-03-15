#ifndef RESOURCECONTROLLER_H
#define RESOURCECONTROLLER_H

#include <string.h>
#include <map>
#include <SDL.h>

#include "renderer.h"

using namespace std;

namespace Ossium
{
    /// Standalone resource controller template class
    template <class resourceType>
    class ResourceController
    {
    public:
        ResourceController<resourceType>()
        {
        };

        /// Loads a resource and adds it to the registry
        bool Load(string guid_path, int* loadArgs = NULL)
        {
            bool success = true;
            resourceType* resource = new resourceType();
            /// Load arguments aren't always necessary - allow resources that don't accept additional load arguments
            if (loadArgs != NULL)
            {
                if (!resource->Load(guid_path, loadArgs))
                {
                    success = false;
                }
            }
            else if (!resource->Load(guid_path))
            {
                success = false;
            }
            if (success)
            {
                /// Add to registry
                registry[guid_path] = resource;
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to load resource '%s'.", guid_path.c_str());
            }
            return success;
        };

        /// Post-load initialisation method for general resources
        bool Initialise(string guid_path)
        {
            resourceType* resource = Find(guid_path);
            if (resource != NULL)
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

        /// Overload for textures
        bool Initialise(string guid_path, Renderer* renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN)
        {
            resourceType* resource = Find(guid_path);
            if (resource != NULL)
            {
                if (resource->Init(renderer, pixelFormatting))
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
        void Free(string guid_path)
        {
            if (registry.find(guid_path) != registry.end())
            {
                if (registry[guid_path] != NULL)
                {
                    delete registry[guid_path];
                }
                registry.erase(guid_path);
            }
        };

        /// Returns pointer to a resource, or NULL if the GUID doesn't exist in the registry
        resourceType* Find(string guid_path)
        {
            if (registry.find(guid_path) == registry.end())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to retrieve resource with GUID '%s'!", guid_path.c_str());
                return NULL;
            }
            return registry[guid_path];
        };

        /// Destroys all resources and clears the registry
        void FreeAll()
        {
            /// Iterate through registry and delete everything
            for (typename map<string, resourceType*>::iterator i = registry.begin(); i != registry.end(); i++)
            {
                if (i->second != NULL)
                {
                    delete i->second;
                }
            }
            registry.clear();
        };

    private:
        /// We definitely don't want to copy instances of this class, for obvious reasons
        ResourceController<resourceType>(const ResourceController& thisCopy);
        ResourceController operator=(const ResourceController& thisCopy);

        /// Lookup registry; key = guid_path, value = pointer to resource
        map<string, resourceType*> registry;

    };

}

#endif // RESOURCECONTROLLER_H
