#include "basics.h"

namespace ossium
{
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

    int clamp(int& n, int min, int max)
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

    string ToString(float n)
    {
        stringstream strStream;
        strStream.str("");
        strStream << n;
        return strStream.str();
    }

    string ToString(int n)
    {
        stringstream strStream;
        strStream.str("");
        strStream << n;
        return strStream.str();
    }

}
