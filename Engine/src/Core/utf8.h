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
#ifndef UTF8_H
#define UTF8_H

#include <SDL2/SDL.h>

#include <string>

namespace Ossium
{

    inline namespace Utilities
    {

        /// Checks if the byte is part of a UTF-8 encoded character or if it is ASCII.
        /**
         *  If the value is the first byte of a UTF-8 character, returns the number of bytes in the character.
         *  If the value is any other part of a UTF-8 character, returns 1.
         *  If the byte is ASCII, returns 0.
         */
        Uint8 CheckUTF8(Uint8 byte);

        /// Returns the complete 4-byte code point of a UTF-8 character.
        Uint32 GetCodepointUTF8(std::string utf8Char);

        /// Converts a UTF-8 character to UCS-2 (effectively the original 16-bit only UNICODE encoding).
        /**
         *  Don't use UCS-2 if you can avoid it. Use UTF-8 instead. This is here for dealing with the shortcomings of other libraries.
         *  Returns 0 if conversion fails or you pass in a null terminator character.
         */
        Uint16 ConvertUTF8ToUCS2(std::string utf8Char);

        /// Returns the number of Unicode code points in a UTF-8 string.
        Uint32 GetLengthUTF8(std::string utf8String);

        /*
        /// TODO
        /// Provides UTF-8 string manipulation methods.
        class String : public string
        {
        public:
            size_t length_utf8();

            String substr_utf8(size_t index, size_t len);

            void erase_utf8(size_t index, size_t len);

            void insert_utf8(size_t index, Uint32 codepoint);
            void insert_utf8(size_t index, string utf8char);

        };
        */

    }

}

#endif // UTF8_H
