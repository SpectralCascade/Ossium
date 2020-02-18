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
#ifndef STRINGCONVERT_H
#define STRINGCONVERT_H

#include <utility>

extern "C"
{
    #include <SDL2/SDL.h>
}

#include "typeinference.h"
#include "jsondata.h"
#include "funcutils.h"

namespace Ossium
{

    inline namespace Utilities
    {

        ///
        /// Internal logging functions, such that the main logging system can be used.
        ///

        namespace Internal
        {

            OSSIUM_EDL void __InternalLogWarn(string text);

        }

        ///
        /// FromString() functions
        ///

        OSSIUM_EDL void FromString(...);

        template<typename T>
        typename enable_if<has_FromString<T>::value, void>::type
        FromString(T& obj, string data)
        {
            obj.FromString(data);
        }

        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_insertable<T>::value && !is_base_of<string, T>::value && !is_enum<T>::value, void>::type
        FromString(T& obj, string data)
        {
            stringstream str;
            str.str("");
            str.str(data);
            if (!(str >> obj))
            {
                Internal::__InternalLogWarn("Failed to convert string '" + data + "' to object data.");
            }
        }

        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_insertable<T>::value && !is_base_of<string, T>::value && is_enum<T>::value, void>::type
        FromString(T& obj, string data)
        {
            FromString((int&)obj, data);
        }

        /// Overload for string types (insertion operator breaks up strings by spaces).
        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_insertable<T>::value && is_base_of<string, T>::value, void>::type
        FromString(T& obj, string data)
        {
            obj = data;
        }

        /// Converts string version of an iterable object into said object (except for strings).
        template<typename T>
        typename enable_if<
            !has_FromString<T>::value &&
            !is_insertable<T>::value &&
            (is_insertable<typename T::value_type>::value || has_FromString<typename T::value_type>::value) &&
            is_range_erasable<T>::value &&
            !is_base_of<string, T>::value,
        void>::type
        FromString(T& obj, string data)
        {
            unsigned int index = 0;
            JString jdata = JString(data);
            for (auto itr = obj.begin(); itr != obj.end(); itr++)
            {
                string strValue = "";
                JString element = jdata.ToElement(index);
                if (element == "\\!EB!\\" && ++itr != obj.end())
                {
                    obj.erase(--itr, obj.end());
                    break;
                }
                FromString(*itr, element);
                index++;
            }
        }

        /// Converts string version of a map into an actual map.
        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_key_value_map<T>::value && is_range_erasable<T>::value && !is_base_of<string, T>::value, void>::type
        FromString(T& obj, string data)
        {
            JSON jdata = JSON();
            if (jdata.Parse(data))
            {
                for (auto itr = jdata.begin(); itr != jdata.end(); itr++)
                {
                    /// Extract the key-value pair data
                    typename T::key_type key;
                    typename T::mapped_type value;
                    FromString(key, itr->first);
                    FromString(value, itr->second);
                    /// Now insert into the map
                    obj[key] = value;
                }
            }
            else
            {
                Internal::__InternalLogWarn("Failed to parse data as JSON map!");
            }
        }

        /// Get SDL_Color from string.
        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_same<SDL_Color, T>::value, void>::type
        FromString(T& obj, string data)
        {
            string value = "";
            size_t index = 0;
            for (auto c : data)
            {
                if (isdigit(c))
                {
                    value += c;
                }
                else if (c == ',')
                {
                    *((Uint8*)(((size_t)&obj) + index)) = (Uint8)ToInt(value);
                    ++index;
                    value = "";
                }
            }
        }

        /// Get SDL_Rect from string.
        template<typename T>
        typename enable_if<!has_FromString<T>::value && is_same<SDL_Rect, T>::value, void>::type
        FromString(T& obj, string data)
        {
            string value = "";
            size_t index = 0;
            for (auto c : data)
            {
                if (isdigit(c) || c == '-')
                {
                    value += c;
                }
                else if (c == ',')
                {
                    *((Sint32*)(((size_t)&obj) + index)) = (Sint32)ToInt(value);
                    ++index;
                    value = "";
                }
            }
        }

        ///
        /// ToString() functions
        ///

        template<typename T>
        typename enable_if<has_ToString<T>::value, string>::type
        ToString(T&& obj)
        {
            return forward<T>(obj).ToString();
        }

        template<typename T>
        typename enable_if<!has_ToString<T>::value && is_insertable<T>::value, string>::type
        ToString(T& obj)
        {
            stringstream str;
            str.str("");
            if (!(str << obj))
            {
                Internal::__InternalLogWarn("Failed to convert string to data.");
            }
            return str.str();
        }

        /// Converts data of objects implementing simple iterators into a string format array (such as vector<int>),
        /// except for strings
        template<typename T>
        typename enable_if<
            !has_ToString<T>::value &&
            !is_insertable<T>::value &&
            (is_insertable<typename T::value_type>::value || has_ToString<typename T::value_type>::value) &&
            !is_base_of<string, T>::value,
        string>::type
        ToString(T& data, typename T::iterator* start = nullptr)
        {
            string dataStream;
            if (start == nullptr || !(*start >= data.begin() && *start < data.end()))
            {
                for (auto i = data.begin(); i != data.end();)
                {
                    dataStream += ToString(*i);
                    if (++i != data.end())
                    {
                        dataStream += string(", ");
                    }
                }
            }
            else
            {
                for (auto i = *start; i != data.end();)
                {
                    dataStream += ToString(*i);
                    if (++i != data.end())
                    {
                        dataStream += string(", ");
                    }
                }
            }
            string converted = string("[") + dataStream + string("]");
            return converted;
        }

        /// Converts a map into a JSON string.
        template<typename T>
        typename enable_if<!has_ToString<T>::value && is_key_value_map<T>::value && !is_base_of<string, T>::value, string>::type
        ToString(T& data)
        {
            JSON jdata = JSON();
            for (auto itr : data)
            {
                /// Convert data to strings
                jdata[ToString(itr.first)] = (JString)ToString(itr.second);
            }
            return jdata.ToString();
        }

        template<typename T>
        typename enable_if<!has_ToString<T>::value && is_same<SDL_Color, T>::value, string>::type
        ToString(T& obj)
        {
            return "(" + ToString((int)obj.r) + ", " + ToString((int)obj.g) + ", " + ToString((int)obj.b) + ", " + ToString((int)obj.a) + ")";
        }

        template<typename T>
        typename enable_if<!has_ToString<T>::value && is_same<SDL_Rect, T>::value, string>::type
        ToString(T& obj)
        {
            return "(" + ToString((int)obj.x) + ", " + ToString((int)obj.y) + ", " + ToString((int)obj.w) + ", " + ToString((int)obj.h) + ")";
        }

        /// Sinkhole for types that ToString() is not implemented for.
        OSSIUM_EDL string ToString(...);

        ///
        /// String formatting function, similar to C# String.Format()
        /// e.g. string name = "Tim";
        /// Format("My first name is {0} and it is {1} characters long!", name, name.length());
        /// would return the string "My first name is Tim and it is 3 characters long!"
        ///

        /// Recursive base case, no arguments
        OSSIUM_EDL void ToStrings(vector<string>& converted);

        /// Base case, no arguments
        OSSIUM_EDL vector<string> ToStrings();

        /// Recursively extracts arguments and appends them to the back of the provided vector
        template<typename T, typename ...Args>
        void ToStrings(vector<string>& converted, T&& value, Args&& ...args)
        {
            converted.push_back(ToString(value));
            ToStrings(converted, forward<Args>(args)...);
        }

        /// Converts all the provided arguments to strings
        template<typename T, typename ...Args>
        vector<string> ToStrings(T&& value, Args&& ...args)
        {
            vector<string> converted;
            converted.push_back(ToString(value));
            ToStrings(converted, forward<Args>(args)...);
            return converted;
        }

        /// Format a string with a list of arguments of various types
        template<typename ...Args>
        string Format(string text, Args&&... args)
        {
            /// Convert arguments to strings
            vector<string> arguments = ToStrings(forward<Args>(args)...);

            if (arguments.empty())
            {
                /// Early out if no arguments
                return text;
            }

            char previous = '\0';
            bool formatting = false;
            string formatted = "";
            string parsing = "";
            for (auto ch : text)
            {
                if (!formatting)
                {
                    if (ch == '{' && previous != '\\')
                    {
                        formatting = true;
                    }
                    else
                    {
                        formatted += ch;
                    }
                }
                else
                {
                    if (ch == '}')
                    {
                        /// Get the argument index
                        if (IsInt(parsing))
                        {
                            int index = -1;
                            FromString(index, parsing);
                            if (index < 0 || (unsigned int)index >= arguments.size())
                            {
                                /// TODO: Log "invalid format argument index".
                            }
                            else
                            {
                                /// Append the argument
                                formatted += arguments[(unsigned int)index];
                            }
                        }
                        else
                        {
                            /// TODO: Log "invalid formatting argument".
                        }
                        formatting = false;
                        parsing = "";
                    }
                    else
                    {
                        parsing += ch;
                    }
                }
                previous = ch;
            }

            return formatted;
        }

    }

}

#endif // STRINGCONVERT_H
