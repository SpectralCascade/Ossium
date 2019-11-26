#include "schemamodel.h"

namespace Ossium
{

    TypeSystem::TypeRegistry<SchemaReferable> SchemaReferable::ref_type_registry;

    Uint32 SchemaReferable::GetReferableType()
    {
        return ref_type_registry.GetType();
    }

}
