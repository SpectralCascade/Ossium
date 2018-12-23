#include "basics.h"

namespace ossium
{
    float clamp(float n, float min, float max)
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

    int clamp(int n, int min, int max)
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

    int wrap(int n, int change, int min, int max)
    {
        int wrapped = n + change;
        if (change > max - min)
        {
            wrapped = n + (change % ((max - min) + 1));
        }
        else if (change < min - max)
        {
            wrapped = n + (change % ((max - min) + 1));
        }
        if (wrapped > max)
        {
            wrapped = min + (wrapped - (max + 1));
        }
        else if (wrapped < min)
        {
            wrapped = max - ((min - 1) - wrapped);
        }
        return wrapped;
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
