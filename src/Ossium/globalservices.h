#ifndef GLOBALSERVICES_H
#define GLOBALSERVICES_H

#include "resourcecontroller.h"
#include "renderer.h"
#include "logging.h"

namespace Ossium
{

    /// Provides static access to various services, e.g. input controller, resources controller
    /// but only for types that inherit from it.
    class GlobalServices
    {
    public:
        GlobalServices();
        virtual ~GlobalServices() = default;

    protected:
        static ResourceController* Resources;
        static Renderer* MainRenderer;
        static Logger Log;

    };

}

#endif // GLOBALSERVICES_H
