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

            OSSIUM_EDL void __InternalLogWarn(std::string text);

        }

        ///
        /// FromString() functions
        ///
        [[deprecated("FromString() reached the variadic sinkhole during SFINAE!")]]
        OSSIUM_EDL void FromString(...);

        template<typename T>
        typename std::enable_if<has_FromString<T>::value, void>::type
        FromString(T& obj, const std::string& data)
        {
            obj.FromString(data);
        }

        template<typename T>
        typename std::enable_if<!has_FromString<T>::value && is_insertable<T>::value && !std::is_base_of<std::string, T>::value && !std::is_enum<T>::value, void>::type
        FromString(T& obj, const std::string& data)
        {
            std::stringstream str;
            str.str("");
            str.str(data);
            if (!(str >> obj))
            {
                //Internal::__InternalLogWarn("Failed to convert string '" + data + "' to object data.");
            }
        }

        template<typename T>
        typename std::enable_if<!has_FromString<T>::value && is_insertable<T>::value && !std::is_base_of<std::string, T>::value && std::is_enum<T>::value, void>::type
        FromString(T& obj, const std::string& data)
        {
            FromString((int&)obj, data);
        }

        /// Overload for string types (insertion operator breaks up strings by spaces).
        template<typename T>
        typename std::enable_if<!has_FromString<T>::value && is_insertable<T>::value && std::is_base_of<std::string, T>::value, void>::type
        FromString(T& obj, const std::string& data)
        {
            obj = data;
        }

        /// Converts string version of a vector object into said object (except for strings).
        template<typename T>
        typename std::enable_if<
            !has_ToString<T>::value &&
            std::is_same<std::vector<typename T::value_type>, T>::value,
        void>::type
        FromString(T& obj, const std::string& data)
        {
            JString jarray = JString(data);
            if (!jarray.IsArray())
            {
                // Early out, clear the target vector.
                printf("WARN: Data passed to FromString() for std::vector is not a valid JString array! Raw data: %s", data.c_str());
                obj.clear();
                return;
            }

            std::vector<JString> jdata = jarray.ToArray();
            auto target = obj.begin();
            unsigned int count = 0;
            for (auto& jstr : jdata)
            {
                if (jstr == "\\!EB!\\")
                {
                    // Early out on failed FromString for an element.
                    if (target < obj.end())
                    {
                        obj.erase(target, obj.end());
                    }
                    break;
                }
                if (target >= obj.end())
                {
                    // This assumes there's a default public constructor available. If not, make it so.
                    typename T::value_type converted;

                    FromString(converted, (std::string)jstr);
                    obj.push_back(converted);
                    target = obj.end();
                }
                else
                {
                    typename T::value_type converted;
                    
                    // Serialise the pre-existing object.
                    FromString(converted, (std::string)jstr);
                    *target = converted;
                    target++;
                }
                count++;
            }

            auto itr = obj.begin() + count;
            if (itr < obj.end())
            {
                obj.erase(itr, obj.end());
            }
        }

        /// Converts string version of a map into an actual map.
        /*template<typename T>
        typename std::enable_if<!has_FromString<T>::value && is_key_value_map<T>::value && is_range_erasable<T>::value && !std::is_base_of<std::string, T>::value, void>::type
        FromString(T& obj, const std::string& data)
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
        }*/

        /// Get SDL_Color from string.
        template<typename T>
        typename std::enable_if<!has_FromString<T>::value && std::is_same<SDL_Color, T>::value, void>::type
        FromString(T& obj, std::string data)
        {
            std::string stripped = Utilities::Strip(Utilities::Strip(data, '('), ')');
            std::vector<std::string> split = Utilities::Split(stripped, ',');
            if (split.size() >= 4)
            {
                obj.r = Utilities::ToInt(split[0]);
                obj.g = Utilities::ToInt(split[1]);
                obj.b = Utilities::ToInt(split[2]);
                obj.a = Utilities::ToInt(split[3]);
            }
        }

        /// Get SDL_Rect from string.
        template<typename T>
        typename std::enable_if<!has_FromString<T>::value && std::is_same<SDL_Rect, T>::value, void>::type
        FromString(T& obj, const std::string& data)
        {
            std::string stripped = Utilities::Strip(Utilities::Strip(data, '('), ')');
            std::vector<std::string> split = Utilities::Split(stripped, ',');
            if (split.size() >= 4)
            {
                obj.x = Utilities::ToInt(split[0]);
                obj.y = Utilities::ToInt(split[1]);
                obj.w = Utilities::ToInt(split[2]);
                obj.h = Utilities::ToInt(split[3]);
            }
        }

        ///
        /// ToString() functions
        ///

        template<typename T>
        typename std::enable_if<has_ToString<T>::value, std::string>::type
        ToString(T&& obj)
        {
            return std::forward<T>(obj).ToString();
        }

        template<typename T>
        typename std::enable_if<!has_ToString<T>::value && is_insertable<T>::value, std::string>::type
        ToString(T& obj)
        {
            std::stringstream str;
            str.str("");
            if (!(str << obj))
            {
                //Internal::__InternalLogWarn("Failed to convert string to data.");
            }
            return str.str();
        }

        /// Converts data of objects implementing simple iterators into a string format array (such as vector<int>),
        /// except for strings
        template<typename T>
        typename std::enable_if<
            !has_ToString<T>::value &&
            !is_insertable<T>::value &&
            (is_insertable<typename T::value_type>::value || has_ToString<typename T::value_type>::value) &&
            !std::is_base_of<std::string, T>::value,
        std::string>::type
        ToString(T& data, typename T::iterator* start = nullptr)
        {
            std::string dataStream;
            if (start == nullptr || !(*start >= data.begin() && *start < data.end()))
            {
                for (auto i = data.begin(); i != data.end();)
                {
                    dataStream += ToString(*i);
                    if (++i != data.end())
                    {
                        dataStream += std::string(", ");
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
                        dataStream += std::string(", ");
                    }
                }
            }
            std::string converted = std::string("[") + dataStream + std::string("]");
            return converted;
        }

        /// Converts a map into a JSON string.
        template<typename T>
        typename std::enable_if<!has_ToString<T>::value && is_key_value_map<T>::value && !std::is_base_of<std::string, T>::value, std::string>::type
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
        typename std::enable_if<!has_ToString<T>::value && std::is_same<SDL_Color, T>::value, std::string>::type
        ToString(T& obj)
        {
            return "(" + ToString((int)obj.r) + ", " + ToString((int)obj.g) + ", " + ToString((int)obj.b) + ", " + ToString((int)obj.a) + ")";
        }

        template<typename T>
        typename std::enable_if<!has_ToString<T>::value && std::is_same<SDL_Rect, T>::value, std::string>::type
        ToString(T& obj)
        {
            return "(" + ToString((int)obj.x) + ", " + ToString((int)obj.y) + ", " + ToString((int)obj.w) + ", " + ToString((int)obj.h) + ")";
        }

        /// Sinkhole for types that ToString() is not implemented for.
        [[deprecated("ToString() reached the variadic sinkhole during SFINAE!")]]
        OSSIUM_EDL std::string ToString(...);

        ///
        /// String formatting function, similar to C# String.Format()
        /// e.g. string name = "Tim";
        /// Format("My first name is {0} and it is {1} characters long!", name, name.length());
        /// would return the string "My first name is Tim and it is 3 characters long!"
        ///

        /// Recursive base case, no arguments
        OSSIUM_EDL void ToStrings(std::vector<std::string>& converted);

        /// Base case, no arguments
        OSSIUM_EDL std::vector<std::string> ToStrings();

        /// Recursively extracts arguments and appends them to the back of the provided vector
        template<typename T, typename ...Args>
        void ToStrings(std::vector<std::string>& converted, T&& value, Args&& ...args)
        {
            converted.push_back(ToString(value));
            ToStrings(converted, std::forward<Args>(args)...);
        }

        /// Converts all the provided arguments to strings
        template<typename T, typename ...Args>
        std::vector<std::string> ToStrings(T&& value, Args&& ...args)
        {
            std::vector<std::string> converted;
            converted.push_back(ToString(value));
            ToStrings(converted, std::forward<Args>(args)...);
            return converted;
        }

        /// Format a string with a list of arguments of various types
        template<typename ...Args>
        std::string Format(std::string text, Args&&... args)
        {
            /// Convert arguments to strings
            std::vector<std::string> arguments = ToStrings(std::forward<Args>(args)...);

            if (arguments.empty())
            {
                /// Early out if no arguments
                return text;
            }

            char previous = '\0';
            bool formatting = false;
            std::string formatted = "";
            std::string parsing = "";
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
