#include "pixeleffects.h"
#include "colors.h"

namespace Ossium
{

    inline namespace graphics
    {

        SDL_Color Grayscale(SDL_Color c, SDL_Point p)
        {
            Uint8 grayscale = (Uint8)(((float)c.r * 0.3f) + ((float)c.g * 0.59f) + ((float)c.b * 0.11f));
            return Color(grayscale, grayscale, grayscale, c.a);
        }

        SDL_Color InvertColor(SDL_Color c, SDL_Point p)
        {
            c.r = c.r > 127 ? 127 - (c.r - 128) : 127 + (128 - c.r);
            c.g = c.g > 127 ? 127 - (c.g - 128) : 127 + (128 - c.g);
            c.b = c.b > 127 ? 127 - (c.b - 128) : 127 + (128 - c.b);
            c.a = c.r == 0 && c.g == 0 && c.b == 0 ? 0x00 : 0xFF;
            return c;
        }

    }

}
