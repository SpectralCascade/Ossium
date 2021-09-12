#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>
#include "stringconvert.h"

#define get [&] ()
#define set [&]

namespace Ossium
{

    // C# style property accessor; only supports the -> operator due to C++ limitations.
    // Also requires definition of both the getter and setter;
    // and setter must have a parameter specified.
    //
    // Example usage:
    //
    //  ExampleType* _example = nullptr;
    //  Property<ExampleType*> example = {
    //      get { Log.Info("Returning example..."); return _example; },
    //      set (ExampleType* const& value) { Log.Info("Setting example..."); return _example = value; }
    //  };
    //
    template<typename T>
    class Property
    {
    public:
        Property(std::function<T()> getfunc, std::function<T(const T&)> setfunc) : getter(getfunc), setter(setfunc) {}

        operator T() const { return getter(); }
        T operator=(const T& value) { return setter(value); }
        T operator->() { return getter(); }

    private:
        std::function<T()> getter;
        std::function<T(const T&)> setter;

    };

}

#endif
