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
#include "utf8.h"

using namespace std;

namespace Ossium
{

    inline namespace Utilities
    {

        Uint8 CheckUTF8(Uint8 byte)
        {
            Uint8 bytes = 0;
            if (byte & 0b10000000)
            {
                bytes++;
                if (byte & 0b01000000)
                {
                    bytes++;
                    if (byte & 0b00100000)
                    {
                        bytes++;
                        if (byte & 0xb00010000)
                        {
                            bytes++;
                        }
                    }
                }
            }
            return bytes;
        }

        Uint32 GetCodepointUTF8(string utf8Char)
        {
            Uint32 codepoint = 0;
            if (utf8Char.empty())
            {
                return 0;
            }

            Uint8 bytes = CheckUTF8(utf8Char[0]);

            if (bytes <= 1)
            {
                codepoint = utf8Char[0];
            }
            else if (bytes == 2)
            {
                codepoint = ((Uint8)utf8Char[0] & 0x1F) << 6 | ((Uint8)utf8Char[1] & 0x3F);
            }
            else if (bytes == 3)
            {
                codepoint = ((Uint8)utf8Char[0] & 0x0F) << 12 | ((Uint8)utf8Char[1] & 0x3F) << 6 | ((Uint8)utf8Char[2] & 0x3F);
            }
            else
            {
                codepoint = ((Uint8)utf8Char[0] & 0x07) << 18 | ((Uint8)utf8Char[1] & 0x3F) << 12 | ((Uint8)utf8Char[2] & 0x3F) << 6 | ((Uint8)utf8Char[3] & 0x3F);
            }
            // Chop off any invalid bits, Unicode only defines 21 bits worth of code points
            codepoint &= 0b00000000000111111111111111111111;

            return codepoint;
        }

        Uint32 GetLengthUTF8(string utf8String)
        {
            Uint32 count = 0;
            for (unsigned int i = 0, counti = utf8String.length(); i < counti;)
            {
                Uint8 byte = CheckUTF8(utf8String[i]);
                i += byte < 1 ? 1 : byte;
                count++;
            }
            return count;
        }

    }

}
