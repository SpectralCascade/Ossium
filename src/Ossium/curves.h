#ifndef CURVES_H
#define CURVES_H

namespace Ossium
{

    inline namespace tweening
    {

        /// Function pointer type for curve/tweening functions. Takes a range (start, end) and the current value in that range.
        /// Should return a normalised floating point number between 0.0f to 1.0f.
        typedef float (*CurveFunction)(float start, float end, float percent);

        /// Linear interpolation.
        float Lerp(float start, float end, float percent);

    }

}

#endif // CURVES_H
