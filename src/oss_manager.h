#ifndef OSS_MANAGER_H
#define OSS_MANAGER_H

#include <vector>
#include <string>
#include <typeinfo>

using namespace std;

/// Manages resources such as textures, spritesheets, audio clips etc.
class OSS_Manager
{
public:
    OSS_Manager(type_info resourceType);
    ~OSS_Manager();

    /// Attempts to add a resource to the data set, returns false on error and deletes resource
    bool addResource(OSS_Resource* resource);

    /// Deletes a specific resource
    bool destroyResource();

    /// Deletes all resources, empties data set
    void freeResources();

    /// Returns a copy of a resource (slower, volatile edits)
    OSS_Resource getResourceCopy(string id);

    /// Returns pointer to a resource (faster, non-volatile edits)
    OSS_Resource* getResource(string id);

    /// Returns the resource type
    type_info getType();

private:
    /// Type of resource e.g. audio clip, texture etc.
    type_info type;

    /// Resources data set
    vector<OSS_Resource*> data;

};

#endif // OSS_MANAGER_H
