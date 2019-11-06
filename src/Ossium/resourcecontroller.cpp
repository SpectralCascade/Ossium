#include "resourcecontroller.h"

namespace Ossium
{

    ResourceController::ResourceController()
    {
        registryByType = new unordered_map<string, Resource*>[typesys::TypeRegistry<Resource>::GetTotalTypes()];
    }

    bool Resource::Init(string args)
    {
        return true;
    }

}
