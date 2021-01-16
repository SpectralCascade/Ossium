#ifndef SCHEMATYPE_H
#define SCHEMATYPE_H

#include "resource.h"

namespace Ossium
{

    /// Not used for anything other than type checking and the resource system.
    class SchemaType : public Resource
    {
    public:
        DECLARE_RESOURCE(SchemaType);

        bool Load(std::string filePath);

    };

}

#endif // SCHEMATYPE_H
