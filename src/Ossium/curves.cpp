#include "curves.h"

#include <SDL.h>

namespace Ossium
{

    inline namespace tweening
    {

        float Lerp(float start, float end, float percent)
        {
            return ((end - start) * percent) + start;
        }

    }

}
