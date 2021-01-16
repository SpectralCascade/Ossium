#ifndef RESOURCE_H
#define RESOURCE_H

#include "typefactory.h"

#include <SDL2/SDL.h>

namespace Ossium
{

    typedef Uint32 ResourceType;

    #define DECLARE_RESOURCE(TYPE)                                                                                      \
    friend class ResourceController;                                                                                    \
    private:                                                                                                            \
        static Resource* ResourceFactory(void* target_controller);                                                      \
    public:                                                                                                             \
        static Ossium::TypeSystem::TypeFactory<Resource, ResourceType> __resource_factory

    #define REGISTER_RESOURCE(TYPE)                                                                                     \
    Resource* TYPE::ResourceFactory(void* target_controller)                                                            \
    {                                                                                                                   \
        return ((ResourceController*)target_controller)->Load<TYPE>("");                                                \
    }                                                                                                                   \
    Ossium::TypeSystem::TypeFactory<Resource, ResourceType> TYPE::__resource_factory(SID( #TYPE )::str, ResourceFactory)

    /// All resource classes e.g. images, audio clips etc. should inherit from this base class
    class Resource
    {
    public:
        virtual ~Resource() = default;
    };

}

#endif // RESOURCE_H
