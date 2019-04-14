#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>
#include <functional>

#include "stringintern.h"
#include "basics.h"
#include "jsondata.h"

using namespace std;

namespace Ossium
{

    ///
    /// Schema
    ///

    template<class BaseType, unsigned int MaximumMembers = 20>
    struct Schema
    {
        const static unsigned int MaxMembers = MaximumMembers;

        typedef unsigned int MemberIdent;

        static unsigned int AddMember(const char* type, const char* name, size_t mem_size,
                                      function<bool(void*, const char*, string)> lambdaFromString, function<string(void*, const char*)> lambdaToString, const char* ultimate_name)
        {
            DEBUG_ASSERT(count < MaximumMembers, "Exceeded maximum number of members. Please allocate a higher maximum in the Schema.");
            member_names[count] = name;
            member_types[count] = type;
            member_byte_sizes[count] = mem_size;
            member_from_string[count] = lambdaFromString;
            member_to_string[count] = lambdaToString;
            schema_name = ultimate_name;
            count++;
            return count - 1;
        }

        /// This root schema has no members of it's own.
        static unsigned int GetMemberCount()
        {
            return 0;
        }

        static const char* GetMemberType(unsigned int index)
        {
            return member_types[index];
        }

        static const char* GetMemberName(unsigned int index)
        {
            return member_names[index];
        }

        void* GetMember(unsigned int index)
        {
            size_t offset = 0;
            for (unsigned int i = 0; i < index; i++)
            {
                offset += member_byte_sizes[i];
            }
            return (void*)((size_t)((void*)this) + offset);
        }

        /// Creates a JSON object representation of this schema using all members of the local schema hierarchy.
        JSON* SerialiseSchema()
        {
            JSON* data = new JSON();
            for (unsigned int i = 0; i < count; i++)
            {
                /// Key consists of type and member name
                string key = member_names[i];
                /// Value is obtained directly from the member
                string value;
                void* member = GetMember(i);
                if (member != nullptr)
                {
                    value = member_to_string[i](member, member_types[i]);
                    /// Add the key-value pair to the JSON object
                    (*data)[key] = value;
                }
                else
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Could not get member '%s' at index [%d] during serialisation!", key.c_str(), i);
                }
            }
            return data;
        }

        /// Sets the values of all members in the local schema hierarchy using a JSON object representation of the schema
        void SerialiseSchema(JSON* data)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                string key = string(member_names[i]);
                auto itr = data->find(key);
                if (itr != data->end())
                {
                    bool success = member_from_string[i](GetMember(i), member_types[i], itr->second);
                    if (!success)
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to serialise member '%s' of type '%s'.", member_names[i], member_types[i]);
                    }
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not find member '%s' of type '%s' in JSON data during serialisation.", member_names[i], member_types[i]);
                }
            }
        }

        /// Returns the ultimate name of this schema
        static const char* GetSchemaName()
        {
            return schema_name;
        }

    protected:
        static size_t member_byte_sizes[MaximumMembers];

    private:
        static function<bool(void*, const char*, string)> member_from_string[MaximumMembers];
        static function<string(void*, const char*)> member_to_string[MaximumMembers];
        static const char* member_names[MaximumMembers];
        static const char* member_types[MaximumMembers];
        /// The name of the final schema in the local schema hierarchy
        static const char* schema_name;
        /// Total members altogether
        static unsigned int count;

    };

    template<class BaseType, unsigned int MaximumMembers>
    function<bool(void*, const char*, string)> Schema<BaseType, MaximumMembers>::member_from_string[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    function<string(void*, const char*)> Schema<BaseType, MaximumMembers>::member_to_string[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    size_t Schema<BaseType, MaximumMembers>::member_byte_sizes[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    const char* Schema<BaseType, MaximumMembers>::member_names[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    const char* Schema<BaseType, MaximumMembers>::member_types[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    const char* Schema<BaseType, MaximumMembers>::schema_name = "";

    template<class BaseType, unsigned int MaximumMembers>
    unsigned int Schema<BaseType, MaximumMembers>::count = 0;

    ///
    /// SchemaRoot
    ///

    class SchemaRoot
    {
    public:
        constexpr static unsigned int GetMemberCount()
        {
            return 0;
        }

        constexpr static void* GetMember(unsigned int index)
        {
            return nullptr;
        }

        constexpr static const char* GetMemberName(unsigned int index)
        {
            return "(null)";
        }

        constexpr static const char* GetMemberType(unsigned int index)
        {
            return "(null)";
        }

        static JSON* SerialiseSchema()
        {
            return new JSON();
        }

        static void SerialiseSchema(JSON* data)
        {
            /// Do nothing
        }

        constexpr static const char* GetSchemaName()
        {
            return "";
        }

        constexpr static unsigned int GetLocalSchemaDepth()
        {
            return 0;
        }

    };

    ///
    /// MemberInfo
    ///

    template<typename SchemaType, typename Type, typename strType, typename strName>
    struct MemberInfo
    {
        MemberInfo(unsigned int& m_count, function<bool(void*, const char*, string)> lambdaFromString, function<string(void*, const char*)> lambdaToString, const char* ultimate_name)
        {
            ++m_count;
            index = SchemaType::AddMember(strType::str, strName::str, sizeof(Type), lambdaFromString, lambdaToString, ultimate_name);
        }

        inline static const char* type = strType::str;
        inline static const char* name = strName::str;

        static unsigned int GetIndex()
        {
            return index;
        }

        Type operator=(const Type& setDefault)
        {
            default_value = setDefault;
            return default_value;
        }

        Type GetDefaultValue()
        {
            return default_value;
        }

        operator Type()
        {
            return default_value;
        }

    private:
        static unsigned int index;

        /// The default value of the member
        Type default_value;

    };

    template<typename SchemaType, typename Type, typename strType, typename strName>
    unsigned int MemberInfo<SchemaType, Type, strType, strName>::index = 0;

    #define DECLARE_SCHEMA(TYPE, BASE_SCHEMA_TYPE)                                                              \
            private: typedef BASE_SCHEMA_TYPE BaseSchemaType;                                                   \
            inline static unsigned int schema_local_count;                                                      \
            constexpr static const char* schema_local_typename = SID(#TYPE)::str;                               \
            public:                                                                                             \
            static unsigned int GetMemberCount()                                                                \
            {                                                                                                   \
                return schema_local_count + BaseSchemaType::GetMemberCount();                                   \
            }                                                                                                   \
            void* GetMember(unsigned int index)                                                                 \
            {                                                                                                   \
                if (index >= BaseSchemaType::GetMemberCount())                                                  \
                {                                                                                               \
                    size_t offset = 0;                                                                          \
                    for (unsigned int i = 0; i < index; i++)                                                    \
                    {                                                                                           \
                        offset += BaseSchemaType::member_byte_sizes[i];                                         \
                    }                                                                                           \
                    return (void*)((size_t)((void*)this) + offset);                                             \
                }                                                                                               \
                return BaseSchemaType::GetMember(index);                                                        \
            }


    ///
    /// Lambda definitions for serialising a particular member in a string format
    ///

    #define MEMBER_FROM_STRING(TYPE)                                        \
    [](void* member, const char* strtype, string data)                      \
    {                                                                       \
        if (strcmp (strtype, SID(#TYPE )::str ) == 0)                       \
        {                                                                   \
            functions::FromString(*(( TYPE *)member), data);                \
            return true;                                                    \
        }                                                                   \
        return false;                                                       \
    }

    #define MEMBER_TO_STRING(TYPE)                                          \
    [](void* member, const char* strtype)                                   \
    {                                                                       \
        if (strcmp(strtype, SID(#TYPE )::str) == 0)                         \
        {                                                                   \
            return functions::ToString(*(( TYPE *)member));                 \
        }                                                                   \
        return string("");                                                  \
    }

    /// This uses the wonderful Construct On First Use idiom to ensure that the order of the members is always base class, then derived class
    #define m(TYPE, NAME)                                                                                                                                                                                           \
            MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >& schema_m_##NAME()                                                                                             \
            {                                                                                                                                                                           \
                static MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >* initialised_info =                                                                                 \
                    new MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >(schema_local_count, MEMBER_FROM_STRING( TYPE ), MEMBER_TO_STRING( TYPE ), schema_local_typename);  \
                return *initialised_info;                                                                                                                                               \
            }                                                                                                                                                                           \
            TYPE NAME = schema_m_##NAME()

    #define CONSTRUCT_SCHEMA(BASETYPE, SCHEMA_TYPE)                                                     \
            private: constexpr static unsigned int local_depth = BASETYPE::GetLocalSchemaDepth() + 1;   \
            public:                                                                                     \
            constexpr static unsigned int GetLocalSchemaDepth()                                         \
            {                                                                                           \
                return local_depth;                                                                     \
            }                                                                                           \
            static unsigned int GetMemberCount()                                                        \
            {                                                                                           \
                return SCHEMA_TYPE::GetMemberCount() + BASETYPE::GetMemberCount();                      \
            }                                                                                           \
            static const char* GetMemberName(unsigned int index)                                        \
            {                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                \
                {                                                                                       \
                    return SCHEMA_TYPE::GetMemberName(index - BASETYPE::GetMemberCount());              \
                }                                                                                       \
                return BASETYPE::GetMemberName(index);                                                  \
            }                                                                                           \
            static const char* GetMemberType(unsigned int index)                                        \
            {                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                \
                {                                                                                       \
                    return SCHEMA_TYPE::GetMemberType(index - BASETYPE::GetMemberCount());              \
                }                                                                                       \
                return BASETYPE::GetMemberType(index);                                                  \
            }                                                                                           \
            void* GetMember(unsigned int index)                                                         \
            {                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                \
                {                                                                                       \
                    return SCHEMA_TYPE::GetMember(index - BASETYPE::GetMemberCount());                  \
                }                                                                                       \
                return BASETYPE::GetMember(index);                                                      \
            }                                                                                           \
            void SerialiseSchema(JSON* data)                                                            \
            {                                                                                           \
                string schema_name = string(SCHEMA_TYPE::GetSchemaName());                              \
                auto itr = data->find(schema_name);                                                     \
                if (itr != data->end())                                                                 \
                {                                                                                       \
                    if (itr->second.IsJSON())                                                           \
                    {                                                                                   \
                        JSON schemaData(static_cast<string>(itr->second));                              \
                        SCHEMA_TYPE::SerialiseSchema(&schemaData);                                      \
                    }                                                                                   \
                }                                                                                       \
                else                                                                                    \
                {                                                                                       \
                    SDL_Log("Warning: could not find key '%s' in JSON.", schema_name.c_str());          \
                }                                                                                       \
                BASETYPE::SerialiseSchema(data);                                                        \
            }                                                                                           \
            JSON* SerialiseSchema()                                                                     \
            {                                                                                           \
                JSON* data = BASETYPE::SerialiseSchema();                                               \
                string schema_name = string(SCHEMA_TYPE::GetSchemaName());                              \
                JSON* schemaData = SCHEMA_TYPE::SerialiseSchema();                                      \
                (*data)[schema_name] = schemaData->ToString();                                          \
                delete schemaData;                                                                      \
                schemaData = nullptr;                                                                   \
                return data;                                                                            \
            }

}

#endif // SCHEMAMODEL_H
