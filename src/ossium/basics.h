#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <sstream>

#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    ///
    /// Handy utility functions
    ///

    /// Clamps a number to maxima/minima
    float clamp(float n, float min, float max);
    int clamp(int n, int min, int max);

    /// Wraps an integer number within a given range
    int wrap(int n, int change, int min, int max);

    /// Maps a value and it's range to a different range
    /// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to
    float mapRange(float value, float min, float max, float min_new, float max_new);

    /// Converts numerical values to strings using stringstream
    string ToString(float n);
    string ToString(int n);

    /// Removes spaces from either end of an input string and returns a new string
    string strip(string data);

    /// Determine whether a stripped ASCII string is an integer
    bool IsInt(const string& data);
    /// Determine whether a stripped ASCII string is a float
    bool IsFloat(const string& data);
    /// Convenient method to check whether a string is numerical at all
    bool IsNumber(const string& data);

    /// Convert a string to int or float using stringstream
    int ToInt(const string& data);
    float ToFloat(const string& data);

    ///
    /// General interfaces and mix-in classes for programming patterns
    ///

    /// Easy to use singleton mix-in; don't use this unless your class could never EVER exist as more than one instance
    template<class Derived>
    class Singleton
    {
    public:
        static Derived& _Instance()
        {
            return singleInstance;
        }

    protected:
        /// The only instance of the derived class
        static Derived singleInstance;

        Singleton(){};

    private:
        NOCOPY(Singleton);
    };

    template<class Derived>
    Derived Singleton<Derived>::singleInstance;

}

#endif // BASICS_H
