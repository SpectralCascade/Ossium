#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>

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

        static unsigned int AddMember(const char* type, const char* name, size_t mem_size)
        {
            DEBUG_ASSERT(count < MaximumMembers, "Exceeded maximum number of members. Please allocate a higher maximum in the Schema.");
            member_names[count] = name;
            member_types[count] = type;
            member_byte_sizes[count] = mem_size;
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

        /// Creates a JSON object representation of this schema. This method gets overridden as you move down the inheritance chain for the schema.
        JSON* CreateJSON()
        {
            return new JSON();
        }

    protected:
        static size_t member_byte_sizes[MaximumMembers];

    private:
        static const char* member_names[MaximumMembers];
        static const char* member_types[MaximumMembers];
        /// Total members altogether
        static unsigned int count;

    };

    template<class BaseType, unsigned int MaximumMembers>
    size_t Schema<BaseType, MaximumMembers>::member_byte_sizes[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    const char* Schema<BaseType, MaximumMembers>::member_names[MaximumMembers];

    template<class BaseType, unsigned int MaximumMembers>
    const char* Schema<BaseType, MaximumMembers>::member_types[MaximumMembers];

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

    };

    ///
    /// MemberInfo
    ///

    template<typename SchemaType, typename Type, typename strType, typename strName>
    struct MemberInfo
    {
        MemberInfo(unsigned int& m_count)
        {
            ++m_count;
            index = SchemaType::AddMember(strType::str, strName::str, sizeof(Type));
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

    /// This uses the wonderful Construct On First Use idiom to ensure that the order of the members is always base class, then derived class
    #define m(TYPE, NAME)                                                                                                                                                                       \
            MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >& schema_m_##NAME()                                                                                                     \
            {                                                                                                                                                                                   \
                static MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >* initialised_info = new MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >(schema_local_count);   \
                return *initialised_info;                                                                                                                                                       \
            }                                                                                                                                                                                   \
            TYPE NAME = schema_m_##NAME()

    #define CONSTRUCT_SCHEMA(BASETYPE, SCHEMA_TYPE)                                         \
            static unsigned int GetMemberCount()                                            \
            {                                                                               \
                return SCHEMA_TYPE::GetMemberCount() + BASETYPE::GetMemberCount();          \
            }                                                                               \
            static const char* GetMemberName(unsigned int index)                            \
            {                                                                               \
                if (index >= BASETYPE::GetMemberCount())                                    \
                {                                                                           \
                    return SCHEMA_TYPE::GetMemberName(index - BASETYPE::GetMemberCount());  \
                }                                                                           \
                return BASETYPE::GetMemberName(index);                                      \
            }                                                                               \
            static const char* GetMemberType(unsigned int index)                            \
            {                                                                               \
                if (index >= BASETYPE::GetMemberCount())                                    \
                {                                                                           \
                    return SCHEMA_TYPE::GetMemberType(index - BASETYPE::GetMemberCount());  \
                }                                                                           \
                return BASETYPE::GetMemberType(index);                                      \
            }                                                                               \
            void* GetMember(unsigned int index)                                             \
            {                                                                               \
                if (index >= BASETYPE::GetMemberCount())                                    \
                {                                                                           \
                    return SCHEMA_TYPE::GetMember(index - BASETYPE::GetMemberCount());      \
                }                                                                           \
                return BASETYPE::GetMember(index);                                          \
            }

}

#endif // SCHEMAMODEL_H
