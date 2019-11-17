#ifndef STRINGCONVERT_H
#define STRINGCONVERT_H

#include <utility>

#include "typeinference.h"
#include "jsondata.h"
#include "basics.h"

namespace Ossium
{

    inline namespace Utilities
    {
        ///
        /// FromString() functions
        ///

        void FromString(...);

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
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to convert string '%s' to object data.", data.c_str());
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
        typename enable_if<!has_FromString<T>::value && !is_insertable<T>::value && is_insertable<typename T::value_type>::value && is_range_erasable<T>::value && !is_base_of<string, T>::value, void>::type
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
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to parse data as JSON map!");
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
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to convert string to data.");
            }
            return str.str();
        }

        /// Converts data of objects implementing simple iterators into a string format array (such as vector<int>),
        /// except for strings
        template<typename T>
        typename enable_if<!has_ToString<T>::value && !is_insertable<T>::value && is_insertable<typename T::value_type>::value && !is_base_of<string, T>::value, string>::type
        ToString(T& data, typename T::iterator* start = nullptr)
        {
            stringstream dataStream;
            dataStream.str("");
            if (start == nullptr || !(*start >= data.begin() && *start < data.end()))
            {
                for (auto i = data.begin(); i != data.end();)
                {
                    dataStream << (*i);
                    if (++i != data.end())
                    {
                        dataStream << ", ";
                    }
                }
            }
            else
            {
                for (auto i = *start; i != data.end();)
                {
                    dataStream << *i;
                    if (++i != data.end())
                    {
                        dataStream << ", ";
                    }
                }
            }
            string converted = string("[") + dataStream.str() + string("]");
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
        string ToString(...);

    }

}

#endif // STRINGCONVERT_H
