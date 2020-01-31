#ifndef UTF8_H
#define UTF8_H

#include <SDL2/SDL.h>

#include <string>

using namespace std;

namespace Ossium
{

    inline namespace Utilities
    {

        /// Checks if the byte is part of a UTF-8 encoded character or if it is ASCII.
        /** If the value is the first byte of a UTF-8 character, returns the number of bytes in the character.
         *  If the value is any other part of a UTF-8 character, returns 1.
         *  If the byte is ASCII, returns 0.
         */
        Uint8 CheckUTF8(Uint8 byte);

        /// Returns the complete 4-byte code point of a UTF-8 character.
        Uint32 GetCodepointUTF8(string utfChar);

        /// Converts a UTF-8 code point to UCS-2 (effectively the original 16-bit only UNICODE encoding).
        /**
         *  Don't use UCS-2 if you can avoid it. Use UTF-8 instead. This is here for dealing with the shortcomings of other libraries.
         *  Returns 0 if conversion fails or you pass in a null terminator character.
         */
        Uint16 ConvertUTF8ToUCS2(string utf8Char);

    }

}

#endif // UTF8_H
