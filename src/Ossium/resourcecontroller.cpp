#include "resourcecontroller.h"

namespace Ossium
{

    ResourceController::ResourceController()
    {
        registryByType = new unordered_map<string, Resource*>[TypeSystem::TypeRegistry<Resource>::GetTotalTypes()];
    }

}
