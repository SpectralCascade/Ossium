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

        /// Converts numerical values to strings using stringstream.
        string ToString(float n);
        string ToString(int n);

        /// Removes spaces from either end of an input string and returns a new string.
        string strip(string data);

        /// Splits a string at the first occurrence of the delimiter and returns the second half.
        /// If an error occurs, these functions return the string outputOnError, or the data string if using the default value for outputOnError.
        string splitPair(string data, char delimiter = ' ', string outputOnError = "%s");
        /// Ditto but returns the first half instead.
        string splitPairFirst(string data, char delimiter = ' ', string outputOnError = "%s");

        /// Determine whether a stripped ASCII string is an integer.
        bool IsInt(const string& data);
        /// Determine whether a stripped ASCII string is a float.
        bool IsFloat(const string& data);
        /// Convenient method to check whether a string is numerical at all.
        bool IsNumber(const string& data);

        /// Convert a string to int or float using stringstream.
        int ToInt(const string& data);
        float ToFloat(const string& data);

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
