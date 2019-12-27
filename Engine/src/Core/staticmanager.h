/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
 *  
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  
 *  3. This notice may not be removed or altered from any source distribution.
 *  
**/
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
