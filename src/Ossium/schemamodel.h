#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>
#include <iostream>

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

    template<class BaseType>
    struct SchemaController
    {
    public:
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

        void AddSchema(Schema<BaseType>* schema)
        {
            schemas[schema_count] = schema;
            schema_count++;
        }

        typedef BaseType base;

    private:
        static unsigned char depth;

        static unsigned int m_count;

        static const char* names[20];
        static const char* types[20];

        Schema<BaseType>* schemas[20];

        static unsigned int schema_count;

    };

    template<typename BaseType>
    unsigned int SchemaController<BaseType>::m_count = 0;

    template<typename BaseType>
    unsigned char SchemaController<BaseType>::depth = 0;

    template<typename BaseType>
    const char* SchemaController<BaseType>::names[20] = {'\0'};

    template<typename BaseType>
    const char* SchemaController<BaseType>::types[20] = {'\0'};

    template<typename BaseType>
    unsigned int SchemaController<BaseType>::schema_count = 0;

    ///
    /// Schema
    ///

    template<class BaseType>
    struct Schema
    {
        Schema(BaseType* controller)
        {
            cout << "Instantiating schema" << endl;
            controller->AddSchema(this);
        }

        static unsigned int AddMember(const char* type, const char* name, size_t mem_size)
        {
            cout << "Adding member '" << name << "' to schema." << endl;
            member_names[count] = name;
            member_types[count] = type;
            member_byte_sizes[count] = mem_size;
            count++;
            cout << "Count is now " << count << endl;
            return SchemaController<BaseType>::AddMember();
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

        void* GetMember(unsigned int index)
        {
            size_t offset = 0;
            for (unsigned int i = 0; i < index; i++)
            {
                offset += member_byte_sizes[i];
            }
            return (void*)((size_t)((void*)this) + offset);
        }

    private:
        static size_t member_byte_sizes[20];
        static const char* member_names[20];
        static const char* member_types[20];
        /// Total members altogether
        static unsigned int count;

    };

    template<class T>
    size_t Schema<T>::member_byte_sizes[20] = {0};

    template<class T>
    const char* Schema<T>::member_names[20];

    template<class T>
    const char* Schema<T>::member_types[20];

    template<class T>
    unsigned int Schema<T>::count = 0;

    ///
    /// MemberInfo
    ///

    template<typename SchemaType, typename Type, typename strType, typename strName>
    struct MemberInfo
    {
        MemberInfo(unsigned short& m_count)
        {
            ++m_count;
            cout << "Instantiating member info" << endl;
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

    #define DECLARE_SCHEMA(TYPE, BASE_SCHEMA_TYPE) private: typedef BASE_SCHEMA_TYPE BaseSchemaType;                    \
            inline static unsigned short schema_local_count = 0;                                                        \
            public:                                                                                                     \
            constexpr static unsigned int GetMemberCount()                                                              \
            {                                                                                                           \
                return (unsigned int)schema_local_count + BASE_SCHEMA_TYPE::GetMemberCount();                           \
            }                                                                                                           \
            TYPE (BaseType* controller) : BASE_SCHEMA_TYPE (controller) {cout << "created schema with " << GetMemberCount() << " members." << endl;}

    #define m(TYPE, NAME) inline static MemberInfo<BaseSchemaType, TYPE , SID(#TYPE ), SID(#NAME ) > schema_m_##NAME = {schema_local_count};   \
            TYPE NAME = schema_m_##NAME

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
                    return SCHEMA_TYPE::GetMember(index - BASETYPE::GetMemberCount());                                  \
                }                                                                                                       \
                return BASETYPE::GetMember(index);                                                                      \
            }                                                                                                           \
            TYPE () : SCHEMA_TYPE (this)

}

#endif // SCHEMAMODEL_H
