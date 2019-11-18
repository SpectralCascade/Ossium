#ifndef GLOBALSERVICES_H
#define GLOBALSERVICES_H

#include "logging.h"

namespace Ossium
{

    /// Provides access to static services such as the logging system
    class GlobalServices
    {
    protected:
        static Logger Log;

    };

}

#endif // GLOBALSERVICES_H
