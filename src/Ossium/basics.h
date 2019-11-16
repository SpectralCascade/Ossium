#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <sstream>
#include <type_traits>
#include <any>
#include <functional>
#include <map>
#include <utility>
#include <SDL.h>

#include "helpermacros.h"
#include "jsondata.h"

using namespace std;

namespace Ossium
{

    ///
    /// TypeRegistry
    ///

    namespace typesys
    {

        /// Used for statically registering types for checking types at runtime.
        template<class BaseType>
        class TypeRegistry
        {
        protected:
            static Uint32 nextTypeIdent;
            Uint32 typeIdent;

        public:
            TypeRegistry()
            {
                typeIdent = nextTypeIdent;
                nextTypeIdent++;
            }

            const Uint32 GetType()
            {
                return typeIdent;
            }

            static Uint32 GetTotalTypes()
            {
                return (Uint32)nextTypeIdent;
            }

            static bool IsValidType(Uint32 id)
            {
                return id < GetTotalTypes();
            }

        };

        template<class BaseType>
        Uint32 TypeRegistry<BaseType>::nextTypeIdent = 0;

        /// Similar to the type registry, but also allows instantiation of specific types and registers type names.
        template<class CoreType, class IdType>
        class TypeFactory : public TypeRegistry<CoreType>
        {
        private:
            typedef function<CoreType*(void*)> FactoryFunc;

            /// Construct-on-first-use idiom to solve static initialisation order fiasco
            static unordered_map<Uint32, FactoryFunc>& gen_map()
            {
                static unordered_map<Uint32, FactoryFunc>* sifmap = new unordered_map<Uint32, FactoryFunc>();
                return *sifmap;
            }
            static unordered_map<string, Uint32>& type_name_map()
            {
                static unordered_map<string, Uint32>* sifmap = new unordered_map<string, Uint32>();
                return *sifmap;
            }
            static unordered_map<Uint32, const char*>& type_id_map()
            {
                static unordered_map<Uint32, const char*>* sifmap = new unordered_map<Uint32, const char*>();
                return *sifmap;
            }

            const char* key;

        public:
            TypeFactory(const char* name, FactoryFunc factory)
            {
                //SDL_Log("Type factory instantiated for type \"%s\" [%d].", name, TypeRegistry<CoreType>::typeIdent);
                gen_map()[TypeRegistry<CoreType>::typeIdent] = factory;
                type_name_map()[name] = TypeRegistry<CoreType>::typeIdent;
                type_id_map()[TypeRegistry<CoreType>::typeIdent] = name;
                key = name;
            }

            static CoreType* Create(IdType typeId, void* args)
            {
                auto itr = gen_map().find(typeId);
                if (itr != gen_map().end())
                {
                    return itr->second(args);
                }
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to create target [type id '%d'] with TypeFactory instance!", typeId);
                return nullptr;
            }

            static CoreType* Create(string targetType, void* args)
            {
                auto itr = type_name_map().find(targetType);
                if (itr != type_name_map().end())
                {
                    return Create(itr->second, args);
                }
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to create target type '%s' with TypeFactory instance!", targetType.c_str());
                return nullptr;
            }

            static string GetName(IdType ident)
            {
                auto itr = type_id_map().find(ident);
                if (itr != type_id_map().end())
                {
                    return itr->second;
                }
                return "";
            }

            static IdType GetId(string name)
            {
                auto itr = type_name_map().find(name);
                if (itr != type_name_map().end())
                {
                    return itr->second;
                }
                return TypeRegistry<CoreType>::GetTotalTypes();
            }

            string GetName()
            {
                return key;
            }

        };

    }

    inline namespace Utilities
    {

        ///
        /// Handy utility functions
        ///

        /// Clamps a number to maxima/minima.
        float clamp(float n, float min = 0, float max = 1);
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
        string FileToString(ifstream& fileStream);

        ///
        /// SFINAE method/type detection
        /// Based primarily on work by Jean Guegant
        /// http://jguegant.github.io/blogs/tech/sfinae-introduction.html
        ///

        inline namespace memberdetection
        {

            #define DETECT_METHOD(METHOD_NAME)                                                                                                                      \
            template<class T>                                                                                                                                       \
            struct has_##METHOD_NAME                                                                                                                                \
            {                                                                                                                                                       \
                template<typename TypeToCheck>                                                                                                                      \
                /** decltype() returns a bool type if the type to check has the method, otherwise substitution fails and the alternative method is used instead. **/\
                static constexpr decltype(declval<TypeToCheck>().METHOD_NAME(), bool())                                                                             \
                check(int)                                                                                                                                          \
                {                                                                                                                                                   \
                    return true;                                                                                                                                    \
                }                                                                                                                                                   \
                                                                                                                                                                    \
                /** If SFINAE takes place, this is the ultimate fallback method as it takes any type. **/                                                           \
                template<typename SubstitutionFailedType>                                                                                                           \
                static constexpr bool check(...)                                                                                                                    \
                {                                                                                                                                                   \
                    return false;                                                                                                                                   \
                }                                                                                                                                                   \
                                                                                                                                                                    \
                /** We have to pass something to the check() method such that it takes precedence over the variadic method, hence passing an int. **/               \
                static constexpr bool value = check<T>(int());                                                                                                      \
                                                                                                                                                                    \
            }

            #define DETECT_METHOD_P(METHOD_NAME, PARAMTYPE)                                                                                                         \
            template<class T>                                                                                                                                       \
            struct has_##METHOD_NAME                                                                                                                                \
            {                                                                                                                                                       \
                template<typename TypeToCheck>                                                                                                                      \
                static constexpr decltype(declval<TypeToCheck>().METHOD_NAME(declval<PARAMTYPE>()), bool())                                                         \
                check(int)                                                                                                                                          \
                {                                                                                                                                                   \
                    return true;                                                                                                                                    \
                }                                                                                                                                                   \
                                                                                                                                                                    \
                template<typename SubstitutionFailedType>                                                                                                           \
                static constexpr bool check(...)                                                                                                                    \
                {                                                                                                                                                   \
                    return false;                                                                                                                                   \
                }                                                                                                                                                   \
                                                                                                                                                                    \
                static constexpr bool value = check<T>(int());                                                                                                      \
                                                                                                                                                                    \
            }

            template<class T>
            struct is_range_erasable
            {
                template<typename TypeToCheck>
                static constexpr decltype(declval<TypeToCheck>().erase(declval<typename TypeToCheck::iterator>(), declval<typename TypeToCheck::iterator>()), bool())
                check(int)
                {
                    return true;
                }

                template<typename SubstitutionFailedType>
                static constexpr bool check(...)
                {
                    return false;
                }

                static constexpr bool value = check<T>(int());

            };

            typedef char DoesNotHave;
            typedef char DoesHave[2];

            DoesNotHave operator<<(const ostream&, const any&);
            DoesNotHave operator>>(const ostream&, const any&);

            DoesHave& CheckInsertionOp(ostream&);
            DoesNotHave CheckInsertionOp(DoesNotHave);

            template<typename T>
            struct is_insertable
            {
                static ostream& s;
                const static T& t;
                const static bool value = sizeof(CheckInsertionOp(s << t)) == sizeof(DoesHave);
            };

            template<typename T>
            struct is_streamable
            {
                static ostream& s;
                const static T& t;
                const static bool value = sizeof(CheckInsertionOp(s >> t)) == sizeof(DoesHave);
            };

            template<typename T, typename U = void>
            struct is_key_value_map : public false_type
            {
            };

            template<typename T>
            struct is_key_value_map<T, void_t<typename T::key_type, typename T::mapped_type, decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>> : public true_type
            {
            };

            DETECT_METHOD(ToString);

            DETECT_METHOD_P(FromString, string);

        }

        /// Removes white space or some other specified character from both ends of a string
        string strip(string data, char optionalChar = ' ');

        /// Splits a string at the first occurrence of the delimiter and returns the second half.
        /// If an error occurs, these functions return the string outputOnError, or the data string if using the default value for outputOnError.
        string splitRight(string data, char delimiter = ' ', string outputOnError = "%s");
        /// Ditto but returns the first half instead.
        string splitLeft(string data, char delimiter = ' ', string outputOnError = "%s");

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

        Singleton() = default;

    private:
        NOCOPY(Singleton);
    };

    template<class Derived>
    Derived Singleton<Derived>::singleInstance;

}

#endif // BASICS_H
