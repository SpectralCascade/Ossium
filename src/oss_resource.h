#ifndef OSS_RESOURCE_H
#define OSS_RESOURCE_H

#include <string.h>

using namespace std;

class OSS_Resource
{
public:
    /// Sets the resource name
    OSS_Resource(string resourceName);

    /// Returns copy of resource name
    string getName();

private:
    /// Resource name
    string name;

};

#endif // OSS_RESOURCE_H
