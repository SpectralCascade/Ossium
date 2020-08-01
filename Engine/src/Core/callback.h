/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
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
#ifndef CALLBACK_H
#define CALLBACK_H

#include <map>
#include <functional>

#include "logging.h"

namespace Ossium
{

    template<typename CallerType>
    class OSSIUM_EDL Callback
    {
    public:
        typedef std::function<void(CallerType&)> RegisteredCallback;

        /// Calls all listeners, passing in a reference to the caller.
        void operator()(CallerType& caller)
        {
            for (auto func : callees)
            {
                func.second(caller);
            }
        }

        /// Registers a listener to this callback. Returns a handle to the registered listener.
        int operator+=(RegisteredCallback callee)
        {
            nextHandle++;
            callees.insert(std::pair<int, RegisteredCallback>(nextHandle, callee));
            return nextHandle;
        }
        /// Unregisters a listener from this callback. The argument should match the value returned from a previous,
        /// corresponding += register call for the listener in question.
        void operator-=(int handle)
        {
            auto itr = callees.empty() ? callees.end() : callees.find(handle);
            if (itr == callees.end())
            {
                Log.Warning("Failed to unregister callback! Provided handle = [{0}]", handle);
                return;
            }
            callees.erase(itr);
        }

        void Clear()
        {
            callees.clear();
        }

    private:
        /// All registered callback functions identified by handle
        std::map<int, RegisteredCallback> callees;

        static int nextHandle;

    };

    template<typename CallerType>
    int Callback<CallerType>::nextHandle = -1;

}

#endif // CALLBACK_H
