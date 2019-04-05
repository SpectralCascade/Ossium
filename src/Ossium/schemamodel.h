#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>
#include <iostream>

#include "basics.h"

using namespace std;

namespace Ossium
{

    template<class SourceClass, unsigned int MaxMembers = 20>
    class Schema
    {
    public:
        static void Add(const char* m_type, const char* member, size_t byteLen)
        {
            types[num] = m_type;
            members[num] = member;
            bytes[num] = byteLen;
            num++;
        }

        static unsigned int Count()
        {
            return num;
        }

        static unsigned int MaxSize()
        {
            return MaxMembers;
        }

        static const char* GetMemberName(unsigned int index)
        {
            if (index >= MaxMembers || members[index] == NULL)
            {
                return "MEMBER INDEX ERROR";
            }
            return members[index];
        }

        static const char* GetMemberType(unsigned int index)
        {
            if (index >= MaxMembers || members[index] == NULL)
            {
                return "MEMBER INDEX ERROR";
            }
            return types[index];
        }

        static size_t GetByteOffset(unsigned int index)
        {
            if (index >= MaxMembers)
            {
                return 0;
            }
            size_t byte_size = 0;
            for (unsigned int i = 0; i < index; i++)
            {
                byte_size += bytes[i];
            }
            return byte_size;
        }

    private:
        /// Array of member types
        static const char* types[MaxMembers];

        /// Array of member names
        static const char* members[MaxMembers];

        /// Byte offsets
        static size_t bytes[MaxMembers];

        /// Total members of the source type
        static unsigned int num;

    };

    template<class SourceClass, unsigned int MaxMembers>
    unsigned int Schema<SourceClass, MaxMembers>::num = 0;

    template<class SourceClass, unsigned int MaxMembers>
    const char* Schema<SourceClass, MaxMembers>::types[MaxMembers];

    template<class SourceClass, unsigned int MaxMembers>
    const char* Schema<SourceClass, MaxMembers>::members[MaxMembers];

    template<class SourceClass, unsigned int MaxMembers>
    size_t Schema<SourceClass, MaxMembers>::bytes[MaxMembers] = {0};

    template<class SchemaType, typename T, typename StrType, typename StrName>
    class MemberInfo
    {
    public:
        MemberInfo()
        {
            SchemaType::Add(StrType::str, StrName::str, sizeof(T) );
        }

        inline static const char* type = StrType::str;
        inline static const char* name = StrName::str;
    };

    #define StartSchema(TYPE) private: typedef Schema< TYPE > MemberData; \
            public: struct TypeScheme_##TYPE

    #define StartSchemaEx(TYPE, LENGTH) private: typedef Schema< TYPE , LENGTH > MemberData; \
            public: struct TypeScheme_##TYPE

    #define m(TYPE, NAME) inline static MemberInfo<MemberData, TYPE , SID(#TYPE ), SID(#NAME ) > schema_m_##NAME ; \
            TYPE NAME

    #define EndSchema(TYPE) schema_##TYPE;                                                  \
            void* GetMember(unsigned int index)                                             \
            {                                                                               \
                return (void*)&(this->schema_##TYPE) + MemberData::GetByteOffset(index);    \
            }

    class Example
    {
    public:
        void SomeMethod()
        {
            SDL_Log("Iterating over %d members:", MemberData::Count());
            for (unsigned int i = 0; i < MemberData::Count(); i++)
            {
                SDL_Log("Found member '%s' of type '%s'", MemberData::GetMemberName(i), MemberData::GetMemberType(i));
                if (MemberData::GetMemberType(i) == SID("float")::str)
                {
                    cout << "Member value is: " << *((float*)GetMember(i)) << endl;
                }
                else if (MemberData::GetMemberType(i) == SID("int")::str)
                {
                    cout << "Member value is: " << *((int*)GetMember(i)) << endl;
                }
                else if (MemberData::GetMemberType(i) == SID("const char*")::str)
                {
                    cout << "Member value is: " << *((const char**)GetMember(i)) << endl;
                }
            }
        }

        StartSchema(Example)
        {
            m(int, foo) = 16;

            m(int, bar) = 888;

            m(const char*, hello) = "hello world!";

            m(float, age) = 19.02f;

            m(int, oh) = 999;

            m(float, more) = 555.3f;

        }
        EndSchema(Example);

    };

}

#endif // SCHEMAMODEL_H
