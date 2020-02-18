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
