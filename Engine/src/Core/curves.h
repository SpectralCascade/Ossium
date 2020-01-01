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
#ifndef CURVES_H
#define CURVES_H

#include "helpermacros.h"

namespace Ossium
{

    inline namespace Tweening
    {

        /// Function pointer type for curve/Tweening functions. Takes a range (start, end) and the current value in that range.
        /// Should return a normalised floating point number between 0.0f to 1.0f.
        typedef float (*CurveFunction)(float start, float end, float percent);

        /// Declares a curve/Tweening function with a specified name. Saves some typing and looks neat.
        #define DECLARE_TWEEN(NAME) OSSIUM_EDL float NAME(float start, float end, float percent)

        /// Linear interpolation.
        DECLARE_TWEEN(Lerp);

        /// Lerp but squared percentage
        DECLARE_TWEEN(Power2);

        /// Lerp but cubed percentage
        DECLARE_TWEEN(Power3);

        /// etc...
        DECLARE_TWEEN(Power4);

        /// etc...
        DECLARE_TWEEN(Power5);

        /// Lerp, but to the power of n.
        float PowerN(float start, float end, float percent, float n = 10.0f);

        /// Complete sine wave cycle.
        DECLARE_TWEEN(Sine);

        /// Half of a sine wave cycle.
        DECLARE_TWEEN(SineHalf);

        /// Quarter of a sine wave cycle.
        DECLARE_TWEEN(SineQuarter);

        /// Complete cosine wave cycle.
        DECLARE_TWEEN(Cosine);

        /// Half of a cosine wave cycle.
        DECLARE_TWEEN(CosineHalf);

        /// Quarter of a cosine wave cycle.
        DECLARE_TWEEN(CosineQuarter);

        /// Overshoots then comes back.
        DECLARE_TWEEN(Overshoot);

    }

}

#endif // CURVES_H
