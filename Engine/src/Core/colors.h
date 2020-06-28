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
        const SDL_Color GRAY        = {127,     127,    127,    255};
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

    SDL_Color operator-(SDL_Color color, int brightness);
    SDL_Color operator+(SDL_Color color, int brightness);

    ///
    /// Convenience functions
    ///

    /// Returns an SDL_Color from raw values
    OSSIUM_EDL SDL_Color Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);

    /// Returns the original colour with a specified alpha value set.
    OSSIUM_EDL SDL_Color Alpha(SDL_Color color, Uint8 a);

    /// Ditto, but const.
    OSSIUM_EDL SDL_Color constexpr ColorConst(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        return {r, g, b, 0xFF};
    }
    /// Ditto, but with custom alpha value.
    OSSIUM_EDL SDL_Color constexpr ColorConst(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        return {r, g, b, a};
    }
    /// Returns an SDL_Color from a 24 bit hex code
    OSSIUM_EDL SDL_Color Color(HexCode rgb);

    OSSIUM_EDL inline bool operator==(const SDL_Color& a, const SDL_Color& b)
    {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.b;
    }

    OSSIUM_EDL inline bool operator!=(const SDL_Color& a, const SDL_Color& b)
    {
        return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.b;
    }

}

#endif // COLORS_H
