#include <cmath>

#include "mathconsts.h"
#include "curves.h"

using namespace std;

namespace Ossium
{

    inline namespace Tweening
    {

        float Lerp(float start, float end, float percent)
        {
            return start + ((end - start) * percent);
        }

        float Power2(float start, float end, float percent)
        {
            return start + ((end - start) * percent * percent);
        }

        float Power3(float start, float end, float percent)
        {
            return start + ((end - start) * percent * percent * percent);
        }

        float Power4(float start, float end, float percent)
        {
            return start + ((end - start) * percent * percent * percent * percent);
        }

        float Power5(float start, float end, float percent)
        {
            return start + ((end - start) * percent * percent * percent * percent * percent);
        }

        float PowerN(float start, float end, float percent, float n)
        {
            return start + ((end - start) * pow(percent, n));
        }

        float Sine(float start, float end, float percent)
        {
            return start + ((end - start) * sin(percent * Constants::pi * 2.0f));
        }

        float SineHalf(float start, float end, float percent)
        {
            return start + ((end - start) * sin(percent * Constants::pi));
        }

        float SineQuarter(float start, float end, float percent)
        {
            return start + ((end - start) * sin(percent * Constants::pi * 0.5f));
        }

        float Cosine(float start, float end, float percent)
        {
            return start + ((end - start) * cos(percent * Constants::pi * 2.0f));
        }

        float CosineHalf(float start, float end, float percent)
        {
            return start + ((end - start) * cos(percent * Constants::pi));
        }

        float CosineQuarter(float start, float end, float percent)
        {
            return start + ((end - start) * cos(percent * Constants::pi * 0.5f));
        }

        float Overshoot(float start, float end, float percent)
        {
            if (percent < 0.5f)
            {
                return Power4(start, end, percent * 2);
            }
            return SineHalf(end, start, percent * 2);
        }

    }

}
