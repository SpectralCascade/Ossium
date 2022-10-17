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
#include <cmath>

#include "colors.h"
#include "funcutils.h"

using namespace std;

namespace Ossium
{

    SDL_Color ConvertToColor(Uint32 pixel, SDL_PixelFormat* pixelFormat)
    {
        SDL_Color output;
        output.r = (Uint8)(((pixel & pixelFormat->Rmask) >> pixelFormat->Rshift) << pixelFormat->Rloss);
        output.g = (Uint8)(((pixel & pixelFormat->Gmask) >> pixelFormat->Gshift) << pixelFormat->Gloss);
        output.b = (Uint8)(((pixel & pixelFormat->Bmask) >> pixelFormat->Bshift) << pixelFormat->Bloss);
        output.a = (Uint8)(((pixel & pixelFormat->Amask) >> pixelFormat->Ashift) << pixelFormat->Aloss);
        return output;
    }

    SDL_Color operator-(SDL_Color c, int brightness)
    {
        c.r = (Uint8)max((int)c.r - brightness, 0);
        c.g = (Uint8)max((int)c.g - brightness, 0);
        c.b = (Uint8)max((int)c.b - brightness, 0);
        return c;
    }

    SDL_Color operator+(SDL_Color c, int brightness)
    {
        c.r = (Uint8)min((int)c.r + brightness, 255);
        c.g = (Uint8)min((int)c.g + brightness, 255);
        c.b = (Uint8)min((int)c.b + brightness, 255);
        return c;
    }

    SDL_Color Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return (SDL_Color){r, g, b, a};
    }

    SDL_Color Color(HexCode rgb)
    {
        // TODO check this bit shifting correctly; shouldn't these shift numbers be in bits, not half bytes?
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return (SDL_Color){(Uint8)((rgb & Colors::MaskRed) >> 6), (Uint8)((rgb & Colors::MaskGreen) >> 4), (Uint8)((rgb & Colors::MaskBlue) >> 2), 0xFF};
        #else
        return (SDL_Color){(Uint8)(rgb & Colors::MaskRed), (Uint8)((rgb & Colors::MaskGreen) << 2), (Uint8)((rgb & Colors::MaskBlue) << 4), 0xFF};
        #endif
    }

    SDL_Color Color(string hexCode)
    {
        Uint32 converted = Utilities::ToUint32FromHex(hexCode);
        switch (hexCode.length())
        {
        case 6:
            converted = converted << 8;
            converted |= 0x000000FF;
        case 8:
            return Color((converted & 0xFF000000) >> 24, (converted & 0x00FF0000) >> 16, (converted & 0x0000FF00) >> 8, converted & 0x000000FF);
        }
        return Colors::Transparent;
    }

    Uint32 ColorToUint32(SDL_Color color, Uint32 format)
    {
        Uint32 output = 0;
        int bpp;
        Uint32 dummy;
        SDL_PixelFormatEnumToMasks(format, &bpp, &dummy, &dummy, &dummy, &dummy);

        // TODO check this works with all bpp values... should probably float divide and ceil to int
        SDL_ConvertPixels(1, 1,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            SDL_PIXELFORMAT_ABGR8888,
#else
            SDL_PIXELFORMAT_RGBA8888,
#endif
            &color, 4, format, &output, bpp / 8
        );
        return output;
/*
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return ((Uint32)color.a || (Uint32)color.b >> 8 || (Uint32)color.g >> 16 || (Uint32)color.r >> 24);
#else
        return ((Uint32)color.a || (Uint32)color.b << 8 || (Uint32)color.g << 16 || (Uint32)color.r << 24);
#endif
*/
    }

    bool IsValidColor(string hexCode)
    {
        if (hexCode.length() != 6 && hexCode.length() != 8)
        {
            return false;
        }
        for (auto c : hexCode)
        {
            if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f'))
            {
                return false;
            }
        }
        return true;
    }

    SDL_Color Alpha(SDL_Color color, Uint8 a)
    {
        color.a = a;
        return color;
    }

}
