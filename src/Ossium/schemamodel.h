#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <SDL.h>

#include "basics.h"

namespace Ossium
{

    template<class SourceClass, unsigned int MaxMembers = 20>
    class MemberInfo
    {
    public:
        static void Add(const char* m_type, const char* member)
        {
            types[num] = m_type;
            members[num] = member;
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

    private:
        /// Array of member types
        static const char* types[MaxMembers];

        /// Array of member names
        static const char* members[MaxMembers];

        /// Total members of the source type
        static unsigned int num;

    };

    template<class SourceClass, unsigned int MaxMembers>
    unsigned int MemberInfo<SourceClass, MaxMembers>::num = 0;

    template<class SourceClass, unsigned int MaxMembers>
    const char* MemberInfo<SourceClass, MaxMembers>::types[MaxMembers];

    template<class SourceClass, unsigned int MaxMembers>
    const char* MemberInfo<SourceClass, MaxMembers>::members[MaxMembers];

    template<class OriginalMemberInfo, typename StrType, typename StrName>
    class MetaMemberInfo
    {
    public:
        MetaMemberInfo()
        {
            OriginalMemberInfo::Add(StrType::str, StrName::str);
        }

        inline static const char* type = StrType::str;
        inline static const char* name = StrName::str;
    };

    #define StartSchema(TYPE) private: typedef MemberInfo< TYPE > MemberData; \
            public: struct TypeScheme_##TYPE

    #define StartSchemaEx(TYPE, LENGTH) private: typedef MemberInfo< TYPE , LENGTH > MemberData; \
            public: struct TypeScheme_##TYPE

    #define m(TYPE, NAME) inline static MetaMemberInfo<MemberData, SID(#TYPE ), SID(#NAME ) > schema_m_##NAME; \
            TYPE NAME

    #define EndSchema(TYPE) schema_##TYPE

    class Example
    {
    public:
        void SomeMethod()
        {
            SDL_Log("The value of foo is: %f | The value of bar is: %s", schema_Example.foo, schema_Example.bar.c_str());
            SDL_Log("Using the schema, foo's type is %s.", schema_Example.schema_m_foo.type);
            SDL_Log("Iterating over %d members:", MemberData::Count());
            for (unsigned int i = 0; i < MemberData::Count(); i++)
            {
                SDL_Log("Found member '%s' of type '%s'", MemberData::GetMemberType(i), MemberData::GetMemberName(i));
            }
        }

        StartSchema(Example)
        {
            m(float, foo) = 16.4f;

            m(string, bar) = "cool beans";
        }
        EndSchema(Example);

    };

}

#endif // SCHEMAMODEL_H
