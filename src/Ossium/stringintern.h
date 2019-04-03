#ifndef STRINGINTERN_H
#define STRINGINTERN_H

#include <cstdio>

namespace Ossium
{

    template<char... data>
    struct InternedString
    {
        static const char str[];
    };

    template<char... data>
    const char InternedString<data...>::str[]{data...};

    template<int Length>
    constexpr char ch(const char (&string_id)[Length], int index)
    {
        return index < Length ? string_id[index] : '\0';
    }

    #define SID(STR) InternedString<ch(STR, 0), ch(STR, 1), ch(STR, 2), ch(STR, 3), ch(STR, 4), ch(STR, 5), ch(STR, 6), ch(STR, 7), ch(STR, 8), ch(STR, 9)>

    typedef const char* StrID;

}

#endif // STRINGINTERN_H
