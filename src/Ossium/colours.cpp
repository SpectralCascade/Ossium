#include "colours.h"

namespace Ossium
{

    SDL_Color ConvertToColour(Uint32 pixel, SDL_PixelFormat* pixelFormat)
    {
        SDL_Color output;
        output.r = (Uint8)(((pixel & pixelFormat->Rmask) >> pixelFormat->Rshift) << pixelFormat->Rloss);
        output.g = (Uint8)(((pixel & pixelFormat->Gmask) >> pixelFormat->Gshift) << pixelFormat->Gloss);
        output.b = (Uint8)(((pixel & pixelFormat->Bmask) >> pixelFormat->Bshift) << pixelFormat->Bloss);
        output.a = (Uint8)(((pixel & pixelFormat->Amask) >> pixelFormat->Ashift) << pixelFormat->Aloss);
        return output;
    }

    SDL_Color Colour(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return (SDL_Color){r, g, b, a};
    }

    SDL_Color Colour(HexCode rgb)
    {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return (SDL_Color){(Uint8)((rgb & colours::RED_MASK) >> 6), (Uint8)((rgb & colours::GREEN_MASK) >> 4), (Uint8)((rgb & colours::BLUE_MASK) >> 2), 0xFF};
        #else
        return (SDL_Color){(Uint8)(rgb & colours::RED_MASK), (Uint8)((rgb & colours::GREEN_MASK) << 2), (Uint8)((rgb & colours::BLUE_MASK) << 4), 0xFF};
        #endif
    }

}
