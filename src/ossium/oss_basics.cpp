#include "oss_basics.h"

float clamp(float& n, float min, float max)
{
    if (n < min)
    {
        return min;
    }
    else if (n > max)
    {
        return max;
    }
    return n;
}
