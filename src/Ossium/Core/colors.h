#ifndef COLORS_H
#define COLORS_H

#include "helpermacros.h"

extern "C"
{
    #include <SDL2/SDL.h>
}

namespace Ossium
{

    typedef Uint32 HexCode;

    namespace Colors
    {

        const SDL_Color WHITE       = {255,     255,    255,    255};
        const SDL_Color BLACK       = {0,       0,      0,      255};
        const SDL_Color RED         = {255,     0,      0,      255};
        const SDL_Color GREEN       = {0,       255,    0,      255};
        const SDL_Color BLUE        = {0,       0,      255,    255};
        const SDL_Color MAGENTA     = {255,     0,      255,    255};
        const SDL_Color CYAN        = {0,       255,    255,    255};
        const SDL_Color YELLOW      = {255,     255,    0,      255};
        const SDL_Color TRANSPARENT = {0,       0,      0,      0};

        const HexCode RED_MASK   = 0xFF0000;
        const HexCode GREEN_MASK = 0x00FF00;
        const HexCode BLUE_MASK  = 0x0000FF;

    }

    /// Converts raw pixel data into an SDL_Colour. Effectively the opposite of the SDL_MapRGBA() function
    OSSIUM_EDL SDL_Color ConvertToColor(Uint32 pixel, SDL_PixelFormat* pixelFormat);

    ///
    /// Convenience functions
    ///

    /// Returns an SDL_Color from raw values
    OSSIUM_EDL SDL_Color Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);
    /// Returns an SDL_Color from a 24 bit hex code
    OSSIUM_EDL SDL_Color Color(HexCode rgb);

}

#endif // COLORS_H
