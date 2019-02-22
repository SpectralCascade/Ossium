#ifndef COLOURS_H
#define COLOURS_H

#include <SDL.h>

namespace Ossium
{

    typedef Uint32 HexCode;

    namespace colours
    {

        const SDL_Color WHITE   = {255,     255,    255,    255};
        const SDL_Color BLACK   = {0,       0,      0,      255};
        const SDL_Color RED     = {255,     0,      0,      255};
        const SDL_Color GREEN   = {0,       255,    0,      255};
        const SDL_Color BLUE    = {0,       0,      255,    255};
        const SDL_Color MAGENTA = {255,     0,      255,    255};
        const SDL_Color CYAN    = {0,       255,    255,    255};
        const SDL_Color YELLOW  = {255,     255,    0,      255};

        const HexCode RED_MASK   = 0xFF0000;
        const HexCode GREEN_MASK = 0x00FF00;
        const HexCode BLUE_MASK  = 0x0000FF;

    }

    /// Converts raw pixel data into an SDL_Colour. Effectively the opposite of the SDL_MapRGBA() function
    SDL_Color ConvertToColour(Uint32 pixel, SDL_PixelFormat* pixelFormat);

    ///
    /// Convenience functions
    ///

    /// Returns an SDL_Color from raw values
    SDL_Color Colour(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);
    /// Returns an SDL_Color from a 24 bit hex code
    SDL_Color Colour(HexCode rgb);

}

#endif // COLOURS_H
