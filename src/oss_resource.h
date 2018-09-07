#ifndef OSS_RESOURCE_H
#define OSS_RESOURCE_H

#include <string>

#include "oss_resourcemanager.h"

/// Base class for all resource types
class OSS_Resource
{
protected:
    /// Load method can only be accessed by OSS_SubResourceManager or this
    virtual bool load(string guid_path) = 0;

};

#endif // OSS_RESOURCE_H
