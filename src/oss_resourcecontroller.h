#ifndef OSS_RESOURCECONTROLLER_H
#define OSS_RESOURCECONTROLLER_H

#include <string.h>
#include <map>
#include <SDL2/SDL.h>

using namespace std;

/// Standalone resource controller template class
template <class resourceType>
class OSS_ResourceController
{
public:
    /// Loads a resource and adds it to the registry
    bool loadResource(string guid_path, int* loadArgs = NULL)
    {
        bool success = true;
        resourceType* resource = new resourceType();
        /// Load arguments aren't always necessary - allow resources that don't accept additional load arguments
        if (loadArgs != NULL)
        {
            if (!resource->load(guid_path, loadArgs))
            {
                success = false;
            }
        }
        else if (!resource->load(guid_path))
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
    bool postLoadInit(string guid_path)
    {
        resourceType* resource = getResource(guid_path);
        if (resource != NULL)
        {
            if (resource->init())
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
    bool postLoadInit(string guid_path, SDL_Renderer* renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN)
    {
        SDL_assert(renderer != NULL);
        resourceType* resource = getResource(guid_path);
        if (resource != NULL)
        {
            if (resource->init(renderer, windowPixelFormat))
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
    void freeResource(string guid_path)
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
    resourceType* getResource(string guid_path)
    {
        if (registry.find(guid_path) == registry.end())
        {
            return NULL;
        }
        return registry[guid_path];
    };

    /// Destroys all resources and clears the registry
    void freeAll()
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
    /// Lookup registry; key = guid_path, value = pointer to resource
    map<string, resourceType*> registry;

};

#endif // OSS_RESOURCECONTROLLER_H
