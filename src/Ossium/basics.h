#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <functional>
#include <map>
#include <SDL.h>

#include "helpermacros.h"
#include "jsondata.h"

using namespace std;

namespace Ossium
{

    inline namespace Utilities
    {

        /// Clamps a number to maxima/minima.
        float Clamp(float n, float min = 0, float max = 1);
        int Clamp(int n, int min, int max);

        /// Wraps an integer number within a given range. Both min and max are INCLUSIVE.
        int Wrap(int n, int change, int min, int max);

        /// Maps a value and it's range to a different range.
        /// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to.
        float MapRange(float value, float min, float max, float min_new, float max_new);

        /// Converts numerical values to strings using stringstream.
        string ToString(float n);
        string ToString(int n);
        /// Turns an entire file stream into a string.
        string FileToString(ifstream& fileStream);

        /// Removes white space or some other specified character from both ends of a string
        string Strip(string data, char optionalChar = ' ');

        /// Splits a string at the first occurrence of the delimiter and returns the second half.
        /// If an error occurs, these functions return the string outputOnError, or the data string if using the default value for outputOnError.
        string SplitRight(string data, char delimiter = ' ', string outputOnError = "%s");
        /// Ditto but returns the first half instead.
        string SplitLeft(string data, char delimiter = ' ', string outputOnError = "%s");

        ///
        /// Type query functions (for converting strings to specific data types).
        ///

        /// Does this string represent a integer value?
        bool IsInt(const string& data);
        /// Does this string represent a floating point value?
        bool IsFloat(const string& data);
        /// Does this string represent a numerical value?
        bool IsNumber(const string& data);
        /// Does this string represent a boolean value?
        bool IsBool(const string& data);
        /// Does this string represent a string value?
        bool IsString(const string& data);

        /// Convert a string to an int
        int ToInt(const string& data);
        /// Convert a string to a float
        float ToFloat(const string& data);
        /// Convert a string to a bool
        bool ToBool(const string& data);

        /// Picks an object from a vector if it meets the specified condition function.
        template<class T>
        T* Pick(vector<T>& data, function<bool(T&)> picker)
        {
            for (auto& item : data)
            {
                if (picker(item))
                {
                    return &item;
                }
            }
            return nullptr;
        }

    }

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

        Singleton() = default;

    private:
        NOCOPY(Singleton);
    };

    template<class Derived>
    Derived Singleton<Derived>::singleInstance;

}

#endif // BASICS_H
