#ifndef STATICMANAGER_H
#define STATICMANAGER_H

#ifdef OSSIUM_EXPORT_DLL

#include "helpermacros.h"

namespace Ossium
{

    namespace Internal
    {

        /// This class manages static variables when compiled as a shared library
        /// such that created statics are unique.
        class OSSIUM_EDL StaticManager
        {
        public:
            /// Returns a pointer to a static object of the specified type.
            /// Constructs the object if it doesn't exist already.
            template<typename T, typename Args...>
            T* Get(Args&& ...args)
            {
            }

        private:
            unordered_set<void*> statics;

        };

        StaticManager& Statics()
        {
            static StaticManager statics;
            return statics;
        }

    }

}
#endif // OSSIUM_EXPORT_DLL

#endif // STATICMANAGER_H
