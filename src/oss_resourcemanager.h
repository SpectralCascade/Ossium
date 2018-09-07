#ifndef OSS_RESOURCEMANAGER_H
#define OSS_RESOURCEMANAGER_H

#include <string.h>
#include <map>
#include <SDL2/SDL.h>

using namespace std;

/// Resource manager template for managing different types of resources
template <class resourceType>
class OSS_ResourceManager
{
public:
    /// Loads a resource and adds it to the registry
    bool loadResource(string guid_path)
    {
        resourceType* resource = new resourceType();
        if (resource->load(guid_path))
        {
            /// Add to registry
            registry[guid_path] = resource;
        }
        else
        {
            return false;
        }
        return true;
    };

    /*/// Similar to loadResource method but specifically for textures
    bool loadTexture(string guid_path, SDL_Renderer* renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN)
    {
        resourceType* texture = new resourceType();
        if (texture->loadImage(guid_path, renderer, windowPixelFormat))
        {
            /// Add to registry
            registry[guid_path] = texture;
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to load texture '%s'.", guid_path.c_str());
            return false;
        }
        return true;
    };*/

    /// Post-load initialisation method
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
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Cannot find resource '%s' for post-load initialisation!", guid_path);
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
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Cannot find resource '%s' for post-load initialisation!", guid_path);
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

#endif // OSS_RESOURCEMANAGER_H
