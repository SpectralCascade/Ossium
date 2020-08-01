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
#ifndef FUNCUTILS_H
#define FUNCUTILS_H

#include <string>
#include <functional>
#include <map>
#include <SDL2/SDL_types.h>

#include "helpermacros.h"
#include "jsondata.h"

namespace Ossium
{

    inline namespace Utilities
    {

        /// Clamps a number to maxima/minima. Both min and max are INCLUSIVE.
        OSSIUM_EDL float Clamp(float n, float min = 0, float max = 1);
        OSSIUM_EDL int Clamp(int n, int min, int max);

        /// Wraps an integer number within a given range. Both min and max are INCLUSIVE.
        OSSIUM_EDL int Wrap(int n, int change, int min, int max);

        /// Maps a value and it's range to a different range.
        /// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to.
        OSSIUM_EDL float MapRange(float value, float min, float max, float min_new, float max_new);

        /// Converts numerical values to strings using stringstream.
        OSSIUM_EDL std::string ToString(float n);
        OSSIUM_EDL std::string ToString(int n);

        /// Turns an entire file stream into a string.
        OSSIUM_EDL std::string FileToString(std::ifstream& fileStream);

        /// Removes white space or some other specified character from both ends of a string
        OSSIUM_EDL std::string Strip(std::string data, char optionalChar = ' ');

        /// Removes the filename from a path.
        OSSIUM_EDL std::string StripFilename(std::string path);

        /// Removes invalid characters from a given file name and strips it.
        OSSIUM_EDL std::string SanitiseFilename(std::string name);

        /// Splits a string at the first occurrence of the delimiter and returns the second half.
        /// If an error occurs, these functions return the string outputOnError, or the data string if using the default value for outputOnError.
        OSSIUM_EDL std::string SplitRight(std::string data, char delimiter = ' ', std::string outputOnError = "%s");
        /// Ditto but returns the first half instead.
        OSSIUM_EDL std::string SplitLeft(std::string data, char delimiter = ' ', std::string outputOnError = "%s");
        /// Splits a string up into smaller strings depending on the delimiter.
        OSSIUM_EDL std::vector<std::string> Split(std::string data, char delimited = ' ');

        ///
        /// Type query functions (for converting strings to specific data types).
        ///

        /// Does this string represent a integer value?
        OSSIUM_EDL bool IsInt(const std::string& data);
        /// Does this string represent a floating point value?
        OSSIUM_EDL bool IsFloat(const std::string& data);
        /// Does this string represent a numerical value?
        OSSIUM_EDL bool IsNumber(const std::string& data);
        /// Does this string represent a boolean value?
        OSSIUM_EDL bool IsBool(const std::string& data);
        /// Does this string represent a string value?
        OSSIUM_EDL bool IsString(const std::string& data);

        /// Convert a decimal string to an int.
        OSSIUM_EDL int ToInt(const std::string& data);
        /// Convert an ASCII hex string to a Uint32.
        OSSIUM_EDL Uint32 ToUint32FromHex(const std::string& data);
        /// Convert a string to a float
        OSSIUM_EDL float ToFloat(const std::string& data);
        /// Convert a string to a bool
        OSSIUM_EDL bool ToBool(const std::string& data);

        /// Picks the first object from a vector that meets the predicate, returning a pointer to it. Returns nullptr if no object meets the predicate.
        template<class T>
        T* Pick(std::vector<T>& data, std::function<bool(T&)> picker)
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

        /// Picks a pointer from a vector if it meets the specified condition function.
        template<class T>
        typename std::enable_if<std::is_pointer<T>::value, T>::type
        PickPointer(std::vector<T>& data, std::function<bool(T)> picker)
        {
            for (auto item : data)
            {
                if (picker(item))
                {
                    return item;
                }
            }
            return nullptr;
        }

    }

    /// Easy to use singleton mix-in; don't use this unless your class could never EVER exist as more than one instance
    template<class Derived>
    class OSSIUM_EDL Singleton
    {
    public:
        /// The only instance of the derived class
        static Derived Instance;

        virtual ~Singleton() = default;

    protected:
        Singleton() = default;

    private:
        NOCOPY(Singleton);
    };

    template<class Derived>
    Derived Singleton<Derived>::Instance;

}

#endif // FUNCUTILS_H
