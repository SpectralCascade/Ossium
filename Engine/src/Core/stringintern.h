/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
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

    #define SID(STR) InternedString<ch(STR, 0), ch(STR, 1), ch(STR, 2), ch(STR, 3), ch(STR, 4), ch(STR, 5), ch(STR, 6), ch(STR, 7), ch(STR, 8), ch(STR, 9), ch(STR, 10), ch(STR, 11), ch(STR, 12), ch(STR, 13), ch(STR, 14), ch(STR, 15), ch(STR, 16), ch(STR, 17), ch(STR, 18), ch(STR, 19), ch(STR, 20), ch(STR, 21), ch(STR, 22), ch(STR, 23), ch(STR, 24), ch(STR, 25), ch(STR, 26), ch(STR, 27), ch(STR, 28), ch(STR, 29), ch(STR, 30), ch(STR, 31), ch(STR, 32)>

    typedef const char* StrID;

}

#endif // STRINGINTERN_H
