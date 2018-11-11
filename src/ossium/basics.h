#ifndef BASICS_H
#define BASICS_H

namespace ossium
{
    /// Clamps a number to maxima/minima
    float clamp(float& n, float min, float max);

    /// Ditto but for integers
    int clamp(int& n, int min, int max);

}

#endif // BASICS_H
