#include "utf8.h"

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

            if (bytes <= 0)
            {
                // Assume ASCII
                codepoint = (Uint32)utf8Char[0];
            }
            else
            {
                for (Uint8 i = bytes - 1; i >= 0; i--)
                {
                    codepoint |= (unsigned char)(utf8Char[bytes - i - 1]) << (i * 8);
                }
            }

            return codepoint;
        }

        Uint16 ConvertUTF8ToUCS2(string utf8Char)
        {
            // TODO: unit test, ∅ (empty set) should be converted to (hex) U+2205 (decimal 8709).

            if (utf8Char.empty())
            {
                return 0;
            }

            Uint16 ucs2Char = 0;
            Uint8 bytes = CheckUTF8((Uint8)utf8Char[0]);

            if (bytes <= 1)
            {
                ucs2Char = utf8Char[0];
            }
            else if (bytes == 2)
            {
                ucs2Char = ((Uint8)utf8Char[0] & 0x1F) << 6 | ((Uint8)utf8Char[1] & 0x3F);
            }
            else if (bytes == 3)
            {
                ucs2Char = ((Uint8)utf8Char[0] & 0x0F) << 12 | ((Uint8)utf8Char[1] & 0x3F) << 6 | ((Uint8)utf8Char[2] & 0x3F);
            }

            return ucs2Char;
        }

    }

}
