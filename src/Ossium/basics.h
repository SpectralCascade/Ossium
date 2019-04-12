#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <sstream>
#include <SDL.h>

#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    inline namespace functions
    {

        ///
        /// Handy utility functions
        ///

        /// Clamps a number to maxima/minima.
        float clamp(float n, float min, float max);
        int clamp(int n, int min, int max);

        /// Wraps an integer number within a given range. Both min and max are INCLUSIVE.
        int wrap(int n, int change, int min, int max);

        /// Maps a value and it's range to a different range.
        /// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to.
        float mapRange(float value, float min, float max, float min_new, float max_new);

        /// Converts a value to a 1 if it is a 0; useful for avoiding division by zero.
        float zeroToOne(float value);

        /// Converts numerical values to strings using stringstream.
        string ToString(float n);
        string ToString(int n);
        /// Turns an entire file stream into a string.
        string ToString(ifstream& fileStream);

        /// Removes white space or some other specified character from both ends of a string
        string strip(string data, char optionalChar = ' ');

        /// Splits a string at the first occurrence of the delimiter and returns the second half.
        /// If an error occurs, these functions return the string outputOnError, or the data string if using the default value for outputOnError.
        string splitPair(string data, char delimiter = ' ', string outputOnError = "%s");
        /// Ditto but returns the first half instead.
        string splitPairFirst(string data, char delimiter = ' ', string outputOnError = "%s");

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

    }

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

    ///
    /// TypeRegistry
    ///

    namespace typesys
    {

        template<class BaseType>
        class TypeRegistry
        {
        private:
            static BaseType nextTypeIdent;
            BaseType typeIdent;

        public:
            TypeRegistry()
            {
                typeIdent = nextTypeIdent;
                nextTypeIdent++;
            }

            const BaseType getType()
            {
                return typeIdent;
            }

            static Uint32 GetTotalTypes()
            {
                return (Uint32)nextTypeIdent;
            }
        };

        template<class BaseType>
        BaseType TypeRegistry<BaseType>::nextTypeIdent = 0;

    }

}

#endif // BASICS_H
