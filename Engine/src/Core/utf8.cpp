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

    }

}
