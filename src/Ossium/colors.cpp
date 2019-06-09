#include "colors.h"

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

    SDL_Color Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return (SDL_Color){r, g, b, a};
    }

    SDL_Color Color(HexCode rgb)
    {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return (SDL_Color){(Uint8)((rgb & Colors::RED_MASK) >> 6), (Uint8)((rgb & Colors::GREEN_MASK) >> 4), (Uint8)((rgb & Colors::BLUE_MASK) >> 2), 0xFF};
        #else
        return (SDL_Color){(Uint8)(rgb & Colors::RED_MASK), (Uint8)((rgb & Colors::GREEN_MASK) << 2), (Uint8)((rgb & Colors::BLUE_MASK) << 4), 0xFF};
        #endif
    }

}
