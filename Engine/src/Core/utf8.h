#ifndef UTF8_H
#define UTF8_H

#include <SDL2/SDL.h>

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

    }

}

#endif // UTF8_H
