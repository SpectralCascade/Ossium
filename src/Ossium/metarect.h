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
            width = 0;
            height = 0;
            direction.x = 0;
            direction.y = 0;
        }

        /// Position at the centre of the rect
        Point position;

        /// Dimensions of the rect
        float width;
        float height;

        /// The direction the rect is rotated towards
        Vector direction;

        /// The origin point around which the rect rotates, as a percentage of the dimensions
        Point origin;

        /// These rotation methods are purely for convenience to save typing long lines of code

        /// Gets the rotation of the rect in degrees
        inline float Rotation()
        {
            return direction.Rotation();
        }
        /// Rotates the rect in degrees
        void Rotate(float degrees)
        {
            direction.Rotate(degrees);
        }
        /// Gets the rotation of the rect in radians
        inline float RotationRad()
        {
            return direction.RotationRad();
        }
        /// Rotates the rect in radians
        void RotateRad(float radians)
        {
            direction.RotateRad(radians);
        }

        /// Returns the SDL_Rect equivalent of the rect
        SDL_Rect GetSDL()
        {
            SDL_Rect output = {(int)round(position.x - (width * 0.5f)), (int)round(position.y - (height * 0.5f)), (int)round(width), (int)round(height)};
            return output;
        }

    };

}

#endif // METARECT_H
