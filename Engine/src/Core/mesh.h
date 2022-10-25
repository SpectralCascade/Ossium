#ifndef MESH_H
#define MESH_H

#include "schemamodel.h"

namespace Ossium
{
    
    struct MeshSchema : public Schema<MeshSchema, 20>
    {
        DECLARE_BASE_SCHEMA(MeshSchema, 20);

    };

    // TODO
    class Mesh : public MeshSchema
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, MeshSchema);
        
    };

}


#endif // MESH_H
