#include <string>
#include <map>
#include <iterator>

#include "oss_resourcemanager.h"

using namespace std;

bool OSS_ResourceManager<resourceType>::loadResource(string guid_path)
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
}

void OSS_ResourceManager<resourceType>::freeResource(string guid_path)
{
    if (registry.find(guid_path) != registry.end())
    {
        if (registry[guid_path] != NULL)
        {
            delete registry[guid_path];
        }
        registry.erase(guid_path);
    }
}

resourceType* OSS_ResourceManager<resourceType>::getResource(string guid_path)
{
    if (registry.find(guid_path) == registry.end())
    {
        return NULL;
    }
    return registry[guid_path];
}

void OSS_ResourceManager<resourceType>::freeAll()
{
    /// Iterate through registry and delete everything
    for (typename map<string, resourceType*>::iterator i = registry.begin(); i != registry.end(); i++)
    {
        if (i->second != NULL)
        {
            delete i->second;
            i = registry.erase(i);
        }
    }
    registry.clear();
}
