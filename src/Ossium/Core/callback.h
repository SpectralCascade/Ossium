#ifndef CALLBACK_H
#define CALLBACK_H

#include <map>
#include <functional>

#include "logging.h"

using namespace std;

namespace Ossium
{

    template<typename CallerType>
    class OSSIUM_EDL Callback
    {
    public:
        typedef function<void(CallerType&)> RegisteredCallback;

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
            callees.insert(pair<int, RegisteredCallback>(nextHandle, callee));
            return nextHandle;
        }
        /// Unregisters a listener from this callback. The argument should match the value returned from a previous,
        /// corresponding += register call for the listener in question.
        void operator-=(int handle)
        {
            auto itr = callees.empty() ? callees.end() : callees.find(handle);
            if (itr == callees.end())
            {
                Logger::EngineLog().Warning("Failed to unregister callback! Provided handle = [{0}]", handle);
                return;
            }
            callees.erase(itr);
        }

    private:
        /// All registered callback functions identified by handle
        map<int, RegisteredCallback> callees;

        static int nextHandle;

    };

    template<typename CallerType>
    int Callback<CallerType>::nextHandle = -1;

}

#endif // CALLBACK_H
