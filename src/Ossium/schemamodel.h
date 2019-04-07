#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>

#include "stringintern.h"
#include "basics.h"

using namespace std;

namespace Ossium
{

    ///
    /// SchemaController
    ///

    /// Forward declaration
    template<class BaseType>
    struct Schema;

    template<class BaseType, unsigned int MaximumMembers = 50, unsigned int MaxSchemaDepth = 20>
    struct SchemaController
    {
    public:
        const static unsigned int MaxMembers = MaximumMembers;

        static unsigned int MemberCount()
        {
            return m_count;
        }

        static unsigned int AddMember()
        {
            m_count++;
            return m_count - 1;
        }

        static void IncrementDepth()
        {
            depth++;
        }

        static unsigned char GetDepth()
        {
            return depth;
        }

        static unsigned int GetMemberCount(unsigned int depth = 0)
        {
            return 0;
        }

        static const char* GetMemberName(unsigned int index)
        {
            return "SCHEMA ERROR";
        }

        static const char* GetMemberType(unsigned int index)
        {
            return "SCHEMA ERROR";
        }

        constexpr static unsigned int GetSchemaDepth()
        {
            return 0;
        }

        void* GetMember(unsigned int index)
        {
            return nullptr;
        }

        template<typename SchemaType>
        void AddSchema(SchemaType* schema)
        {
            schemas[schema_count] = schema;
            schema_count++;
        }

        typedef BaseType base;

    private:
        static unsigned char depth;

        static unsigned int m_count;

        void* schemas[MaxSchemaDepth];

        static unsigned int schema_count;

    };

    template<typename BaseType, unsigned int MaximumMembers, unsigned int MaxSchemaDepth>
    unsigned int SchemaController<BaseType, MaximumMembers, MaxSchemaDepth>::m_count = 0;

    template<typename BaseType, unsigned int MaximumMembers, unsigned int MaxSchemaDepth>
    unsigned char SchemaController<BaseType, MaximumMembers, MaxSchemaDepth>::depth = 0;

    template<typename BaseType, unsigned int MaximumMembers, unsigned int MaxSchemaDepth>
    unsigned int SchemaController<BaseType, MaximumMembers, MaxSchemaDepth>::schema_count = 0;

    ///
    /// Schema
    ///

    template<class BaseType>
    struct Schema
    {
        Schema(BaseType* controller)
        {
            controller->AddSchema(this);
        }

        static unsigned int AddMember(const char* type, const char* name, size_t mem_size)
        {
            DEBUG_ASSERT(count < BaseType::MaxMembers, "Exceeded maximum number of members. Please allocate a higher maximum in the SchemaController.");
            member_names[count] = name;
            member_types[count] = type;
            member_byte_sizes[count] = mem_size;
            count++;
            return BaseType::base::AddMember();
        }

        static size_t GetMemberOffset(unsigned int index)
        {
            return member_byte_sizes[index];
        }

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

        void* GetMember(unsigned int index, unsigned int depth_count = 0)
        {
            size_t offset = 0;
            for (unsigned int i = 0; i < index; i++)
            {
                offset += member_byte_sizes[i];
            }
            return (void*)((size_t)((void*)this) + offset);
        }

    protected:
        static size_t member_byte_sizes[BaseType::MaxMembers];

    private:
        static const char* member_names[BaseType::MaxMembers];
        static const char* member_types[BaseType::MaxMembers];
        /// Total members altogether
        static unsigned int count;

    };

    template<class BaseType>
    size_t Schema<BaseType>::member_byte_sizes[BaseType::MaxMembers] = {0};

    template<class BaseType>
    const char* Schema<BaseType>::member_names[BaseType::MaxMembers];

    template<class BaseType>
    const char* Schema<BaseType>::member_types[BaseType::MaxMembers];

    template<class BaseType>
    unsigned int Schema<BaseType>::count = 0;

    ///
    /// MemberInfo
    ///

    template<typename SchemaType, typename Type, typename strType, typename strName>
    struct MemberInfo
    {
        MemberInfo(unsigned short& m_count)
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
            inline static unsigned short schema_local_count;                                                    \
            public:                                                                                             \
            constexpr static unsigned int GetMemberCount()                                                      \
            {                                                                                                   \
                return (unsigned int)schema_local_count + BaseSchemaType::GetMemberCount();                     \
            }                                                                                                   \
            void* GetMember(unsigned int index, unsigned int depth_count)                                       \
            {                                                                                                   \
                if (index - depth_count >= BaseSchemaType::GetMemberCount())                                    \
                {                                                                                               \
                    size_t offset = 0;                                                                          \
                    for (unsigned int i = depth_count; i < index; i++)                                          \
                    {                                                                                           \
                        offset += BaseSchemaType::member_byte_sizes[i];                                         \
                    }                                                                                           \
                    return (void*)((size_t)((void*)this) + offset);                                             \
                }                                                                                               \
                return BaseSchemaType::GetMember(index, depth_count);                                           \
            }                                                                                                   \
            TYPE (BaseType* controller) : BaseSchemaType(controller) {}

    /// This uses the wonderful Construct On First Use idiom to ensure that the order of the members is always base class, then derived class
    #define m(TYPE, NAME)                                                                                                                                                                       \
            MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >& schema_m_##NAME()                                                                                                     \
            {                                                                                                                                                                                   \
                static MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >* initialised_info = new MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) >(schema_local_count);   \
                return *initialised_info;                                                                                                                                                       \
            }                                                                                                                                                                                   \
            TYPE NAME = schema_m_##NAME()

    #define CONSTRUCT_SCHEMA(TYPE, BASETYPE, SCHEMA_TYPE)                                                               \
            private: inline const static unsigned int schema_depth = BASETYPE::GetSchemaDepth() + 1;                    \
            public:                                                                                                     \
            constexpr static unsigned int GetSchemaDepth()                                                              \
            {                                                                                                           \
                return schema_depth;                                                                                    \
            }                                                                                                           \
            static unsigned int GetMemberCount()                                                                        \
            {                                                                                                           \
                return SCHEMA_TYPE::GetMemberCount() + BASETYPE::GetMemberCount();                                      \
            }                                                                                                           \
            static const char* GetMemberName(unsigned int index)                                                        \
            {                                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                                \
                {                                                                                                       \
                    return SCHEMA_TYPE::GetMemberName(index);                                                           \
                }                                                                                                       \
                return BASETYPE::GetMemberName(index);                                                                  \
            }                                                                                                           \
            static const char* GetMemberType(unsigned int index)                                                        \
            {                                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                                \
                {                                                                                                       \
                    return SCHEMA_TYPE::GetMemberType(index);                                                           \
                }                                                                                                       \
                return BASETYPE::GetMemberType(index);                                                                  \
            }                                                                                                           \
            void* GetMember(unsigned int index)                                                                         \
            {                                                                                                           \
                if (index >= BASETYPE::GetMemberCount())                                                                \
                {                                                                                                       \
                    return SCHEMA_TYPE::GetMember(index, BASETYPE::GetMemberCount());                                   \
                }                                                                                                       \
                return BASETYPE::GetMember(index);                                                                      \
            }                                                                                                           \
            TYPE () : SCHEMA_TYPE (this)

}

#endif // SCHEMAMODEL_H
