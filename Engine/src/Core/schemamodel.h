/** COPYRIGHT NOTICE
 *  
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
#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

extern "C"
{
    #include <SDL2/SDL.h>
}
#include <fstream>

#include "stringintern.h"
#include "funcutils.h"
#include "typefactory.h"
#include "jsondata.h"

using namespace std;

namespace Ossium
{

    ///
    /// Schema
    ///

    template<class BaseType, unsigned int MaximumMembers = 20>
    struct OSSIUM_EDL Schema
    {
        const static unsigned int MaxMembers = MaximumMembers;

        typedef unsigned int MemberIdent;

        static unsigned int AddMember(const char* type, const char* name, size_t mem_size, int mem_attribute,
                                      function<bool(void*, const char*, string)> lambdaFromString, function<string(void*, const char*)> lambdaToString, const char* ultimate_name)
        {
            DEBUG_ASSERT(count < MaximumMembers, "Exceeded maximum number of members. Please allocate a higher maximum in the Schema.");
            member_names[count] = name;
            member_types[count] = type;
            member_attributes[count] = mem_attribute;
            member_byte_offsets[count] = mem_size;
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

        static int GetMemberAttribute(unsigned int index)
        {
            return member_attributes[index];
        }

        void* GetMember(unsigned int index)
        {
            return (void*)((size_t)((void*)this) + member_byte_offsets[index]);
        }

        void FromString(string& str)
        {
            JSON data(str);
            SerialiseIn(data);
        }

        /// Creates a JSON string with all the schema members.
        string ToString()
        {
            JSON data;
            SerialiseOut(data);
            return data.ToString();
        }

        bool Load(string filePath)
        {
            ifstream file(filePath);
            string data = Utilities::FileToString(file);
            if (!data.empty())
            {
                FromString(data);
                file.close();
                return true;
            }
            Logger::EngineLog().Warning("Failed to load schema from file '{0}'!", filePath);
            return false;
        }

        bool Save(string filePath)
        {
            ofstream file(filePath);
            if (file.is_open())
            {
                file << ToString();
                file.close();
                return true;
            }
            return false;
        }

        /// Creates key-values pairs using all members of the local schema hierarchy with the provided JSON object.
        void SerialiseOut(JSON& data)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                /// Key consists of type and member name
                string key = member_names[i];
                /// Value is obtained directly from the member
                void* member = GetMember(i);
                if (member != nullptr)
                {
                    /// Add the key-value pair to the JSON object
                    data[key] = member_to_string[i](member, member_types[i]);
                }
                else
                {
                    Logger::EngineLog().Error("Could not get member '{0}' at index [{1}] during serialisation!", key, i);
                }
            }
        }

        /// Sets the values of all members in the local schema hierarchy using a JSON object representation of the schema
        void SerialiseIn(JSON& data)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                string key = string(member_names[i]);
                auto itr = data.find(key);
                if (itr != data.end())
                {
                    bool success = member_from_string[i](GetMember(i), member_types[i], itr->second);
                    if (!success)
                    {
                        Logger::EngineLog().Warning("Failed to serialise member '{0}' of type '{1}'.", member_names[i], member_types[i]);
                    }
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not find member '%s' of type '%s' in provided JSON data during serialisation.", member_names[i], member_types[i]);
                }
            }
        }

        /// Returns the ultimate name of this schema
        static const char* GetSchemaName()
        {
            return schema_name;
        }

    protected:
        static size_t member_byte_offsets[MaximumMembers];

        virtual void MapReference(string identdata, void** member)
        {
            Logger::EngineLog().Warning("Reference mapping not implemented for a type deriving from schema type \"{0}\"", schema_name);
        }

    private:
        static function<bool(void*, const char*, string)> member_from_string[MaximumMembers];
        static function<string(void*, const char*)> member_to_string[MaximumMembers];
        /// Array of associated user attributes for each schema member
        static int member_attributes[MaximumMembers];
        /// Array of names for each schema member
        static const char* member_names[MaximumMembers];
        /// Array of type names for each schema member
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
    int Schema<BaseType, MaximumMembers>::member_attributes[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    size_t Schema<BaseType, MaximumMembers>::member_byte_offsets[MaximumMembers];

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

    class OSSIUM_EDL SchemaRoot
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

        static void SerialiseOut(JSON& data)
        {
        }

        static void SerialiseIn(JSON& data)
        {
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
    struct OSSIUM_EDL MemberInfo
    {
        MemberInfo(unsigned int& m_count, function<bool(void*, const char*, string)> lambdaFromString, function<string(void*, const char*)> lambdaToString, const char* ultimate_name, size_t member_offset, int mem_attribute)
        {
            ++m_count;
            index = SchemaType::AddMember(strType::str, strName::str, member_offset, mem_attribute, lambdaFromString, lambdaToString, ultimate_name);
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

    /// Types that can be referenced via pointers should inherit from this CRTP mix-in class.
    class OSSIUM_EDL SchemaReferable
    {
    public:
        virtual ~SchemaReferable() = default;

        virtual string GetReferenceID() = 0;

        Uint32 GetReferableType();

    private:
        static TypeSystem::TypeRegistry<SchemaReferable> ref_type_registry;

    };

    #define DECLARE_SCHEMA(TYPE, BASE_SCHEMA_TYPE)                                                              \
            private: typedef BASE_SCHEMA_TYPE BaseSchemaType;                                                   \
            inline static unsigned int schema_local_count;                                                      \
            inline static TYPE* schema_layout_ref;                                                              \
            constexpr static const char* schema_local_typename = SID(#TYPE)::str;                               \
            public:                                                                                             \
            static unsigned int GetMemberCount()                                                                \
            {                                                                                                   \
                return schema_local_count + BaseSchemaType::GetMemberCount();                                   \
            }

    /// When you want to specify the maximum number of members of a base schema, use this macro instead e.g.
    /// DECLARE_BASE_SCHEMA(ExampleType, 5) will generate schema code for a base type of Schema<ExampleType, 5>
    /// This is necessary as DECLARE_SCHEMA() can't handle Schema<ExampleType, 5> due to the comma separator.
    #define DECLARE_BASE_SCHEMA(TYPE, MAX_MEMBERS)                                                              \
            private: typedef Schema<TYPE, MAX_MEMBERS> BaseSchemaType;                                          \
            inline static unsigned int schema_local_count;                                                      \
            inline static TYPE* schema_layout_ref;                                                              \
            constexpr static const char* schema_local_typename = SID(#TYPE)::str;                               \
            public:                                                                                             \
            static unsigned int GetMemberCount()                                                                \
            {                                                                                                   \
                return schema_local_count + BaseSchemaType::GetMemberCount();                                   \
            }

    ///
    /// Lambda definitions for serialising a particular member in a string format
    ///

    #define MEMBER_FROM_STRING(TYPE)                                        \
    [](void* member, const char* strtype, string data)                      \
    {                                                                       \
        if (strcmp (strtype, SID(#TYPE )::str ) == 0)                       \
        {                                                                   \
            Utilities::FromString(*(( TYPE *)member), data);                \
            return true;                                                    \
        }                                                                   \
        return false;                                                       \
    }

    #define MEMBER_TO_STRING(TYPE)                                          \
    [](void* member, const char* strtype)                                   \
    {                                                                       \
        if (strcmp(strtype, SID(#TYPE )::str) == 0)                         \
        {                                                                   \
            return Utilities::ToString(*(( TYPE *)member));                 \
        }                                                                   \
        return string("");                                                  \
    }

    /// This doesn't actually convert ids to valid pointers; it merely attempts to map
    /// the ids to valid pointers. The relevant ECS object does that once all objects are serialised.
    /// Note schema members only support pointer types to Entity and Component-derived types, if the type is invalid the member is made null.
    #define REFPTR_FROM_STRING(TYPE)                                                                                                            \
    [this](void* member, const char* strtype, string data)                                                                                      \
    {                                                                                                                                           \
        if (strcmp(strtype, SID(#TYPE )::str) == 0)                                                                                             \
        {                                                                                                                                       \
            if (!data.empty() && data != "null")                                                                                                \
            {                                                                                                                                   \
                if (is_base_of<SchemaReferable, remove_pointer<TYPE>::type>::value)                                                             \
                {                                                                                                                               \
                    MapReference(data, (void**)member);                                                                                         \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    Logger::EngineLog().Warning("Type \"{0}\" is not SchemaReferable.", strtype);                                               \
                }                                                                                                                               \
            }                                                                                                                                   \
            (*((void**)member)) = nullptr;                                                                                                      \
            return true;                                                                                                                        \
        }                                                                                                                                       \
        return false;                                                                                                                           \
    }

    /// TODO: Optimise component id search, maybe store last known index of the component as id
    /// e.g. upon creation localId = index? Only caveat is if components of the same type are removed,
    /// the id becomes invalid.
    #define REFPTR_TO_STRING(TYPE)                                                                          \
    [](void* member, const char* strtype)                                                                   \
    {                                                                                                       \
        if (strcmp(strtype, SID(#TYPE )::str) == 0)                                                         \
        {                                                                                                   \
            if ((*((void**)member)) == nullptr)                                                             \
            {                                                                                               \
                return string("null");                                                                      \
            }                                                                                               \
            else if (is_base_of<SchemaReferable, remove_pointer<TYPE>::type>::value)                        \
            {                                                                                               \
                return (*((SchemaReferable**)member))->GetReferenceID();                                    \
            }                                                                                               \
            Logger::EngineLog().Warning("Invalid schema member reference type \"{0}\".", strtype);  \
            return Utilities::ToString(*(( TYPE *)member));                                                 \
        }                                                                                                   \
        return string("null");                                                                              \
    }

    /// This uses the wonderful Construct On First Use idiom to ensure that the order of the members is always base class, then derived class
    /// Also checks if the type is a pointer. If so, it gets the custom TO_STRING and FROM_STRING macros.
    #define SCHEMA_MEMBER(ATTRIBUTE, TYPE, NAME)                                                                                                                        \
            MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >& schema_m_##NAME()                                                                             \
            {                                                                                                                                                           \
                static MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >* initialised_info = is_pointer<TYPE>::value ?                                       \
                    new MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >(schema_local_count,                                                                \
                                    REFPTR_FROM_STRING( TYPE ), REFPTR_TO_STRING( TYPE ), schema_local_typename, (size_t)((void*)&schema_layout_ref->NAME), ATTRIBUTE)  \
                    :                                                                                                                                                   \
                    new MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >(schema_local_count,                                                                \
                                    MEMBER_FROM_STRING( TYPE ), MEMBER_TO_STRING( TYPE ), schema_local_typename, (size_t)((void*)&schema_layout_ref->NAME), ATTRIBUTE); \
                return *initialised_info;                                                                                                                               \
            }                                                                                                                                                           \
            TYPE NAME = schema_m_##NAME()

    /// Shorthand macro that doesn't take an attribute.
    #define M(TYPE, NAME) SCHEMA_MEMBER(0, TYPE, NAME)

    /// Defines a bunch of necessary methods and members for the schema hierarchy to work with the inheritance hierarchy.
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
            void SerialiseIn(JSON& data)                                                                \
            {                                                                                           \
                SCHEMA_TYPE::SerialiseIn(data);                                                         \
                BASETYPE::SerialiseIn(data);                                                            \
            }                                                                                           \
            void SerialiseOut(JSON& data)                                                               \
            {                                                                                           \
                BASETYPE::SerialiseOut(data);                                                           \
                SCHEMA_TYPE::SerialiseOut(data);                                                        \
            }                                                                                           \
            virtual string ToString()                                                                   \
            {                                                                                           \
                JSON data;                                                                              \
                SerialiseOut(data);                                                                     \
                return data.ToString();                                                                 \
            }                                                                                           \
            virtual void FromString(string& str)                                                        \
            {                                                                                           \
                JSON data = JSON(str);                                                                  \
                SerialiseIn(data);                                                                      \
            }

}

#endif // SCHEMAMODEL_H
