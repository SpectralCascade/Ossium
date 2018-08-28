#include <string.h>

#include "oss_resource.h"

OSS_Resource::OSS_Resource(string resourceName)
{
    name = resourceName;
}

string OSS_Resource::getName()
{
    return name;
}
