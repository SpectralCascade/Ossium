#ifndef PIXELEFFECTS_H
#define PIXELEFFECTS_H

#include <SDL.h>

namespace Ossium
{

    inline namespace Graphics
    {

        /// Macro for short hand declarations
        #define DECLARE_PIXEL_EFFECT(NAME) SDL_Color NAME(SDL_Color c, SDL_Point p)

        /// Grayscale effect
        DECLARE_PIXEL_EFFECT(Grayscale);

        /// Inverts the color of a pixel
        DECLARE_PIXEL_EFFECT(InvertColor);

    }

}

#endif // PIXELEFFECTS_H
