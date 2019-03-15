#ifndef METARECT_H
#define METARECT_H

#include <cmath>

#include "primitives.h"

namespace Ossium
{

    /// Some sort of magical rectangle with basic positioning and rotation information...
    class MetaRect
    {
    public:
        MetaRect()
        {
            position.x = 0;
            position.y = 0;
            origin.x = 0.5f;
            origin.y = 0.5f;
        }

        /// Position at the centre of the rect
        Point position;

        /// Dimensions of the rect
        float width = 0;
        float height = 0;

        /// The angle the rect is rotated by.
        float angle = 0;

        /// The origin point around which the rect rotates, as a percentage of the dimensions
        Point origin;

        /// Returns the SDL_Rect equivalent of the rect
        SDL_Rect GetSDL()
        {
            SDL_Rect output = {(int)round(position.x - (width * 0.5f)), (int)round(position.y - (height * 0.5f)), (int)round(width), (int)round(height)};
            return output;
        }

        Rect GetRect()
        {
            return Rect(position.x - (width * 0.5f), position.y - (height * 0.5f), width, height);
        }

    };

}

#endif // METARECT_H
