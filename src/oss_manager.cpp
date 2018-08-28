#include <string.h>
#include <typeinfo>
#include <algorithm>

#include "oss_manager.h"
#include "oss_resource.h"

using namespace std;

OSS_Manager::OSS_Manager(type_info resourceType)
{
    type = resourceType;
}

OSS_Manager::~OSS_Manager()
{
    freeResources();
}

bool OSS_Manager::addResource(OSS_Resource* resource)
{
    SDL_assert(resource != NULL);

    bool success = true;
    /// First check that the resource is of the correct type
    if (type == typeid(&resource))
    {
        /// Now check for resource name conflicts; this shouldn't happen often so linear search is OK
        for (unsigned int i = 0; i < data.size(); i++)
        {
            if (resource->getId() == data[i]->getId())
            {
                /// Resource name conflict
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, ("Resource ID conflict! Resource ID: %s", resource->getId()));
                success = false;
                break;
            }
        }
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, ("Resource typeid '%s' does not match manager type_info '%s'", typeid(&resource), type));
        success = false;
    }

    if (success)
    {
        /// No resource conflicts and of correct type, so attempt to add resource to dataset
        data.push_back(resource);
    }
    else
    {
        /// Delete the resource
        delete resource;
        resource = NULL;
    }

    return success;
}

void OSS_Manager::freeResources()
{
    for (unsigned int i = 0; i < data.size(); i++)
    {
        delete data[i];
    }
    data.clear();
    data.resize(0);
}

OSS_Resource OSS_Manager::getResourceCopy(string id)
{
    /// Search for resource
    if (find(data.begin(), data.end(), )
}
