#ifndef CURVES_H
#define CURVES_H

namespace Ossium
{

    inline namespace Tweening
    {

        /// Function pointer type for curve/tweening functions. Takes a range (start, end) and the current value in that range.
        /// Should return a normalised floating point number between 0.0f to 1.0f.
        typedef float (*CurveFunction)(float start, float end, float percent);

        /// Declares a curve/tweening function with a specified name. Saves some typing and looks neat.
        #define DECLARE_TWEEN(NAME) float NAME(float start, float end, float percent)

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
