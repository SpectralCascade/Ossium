#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <sstream>

using namespace std;

namespace ossium
{
    /// Clamps a number to maxima/minima
    float clamp(float& n, float min, float max);
    int clamp(int& n, int min, int max);

    /// Wraps an integer number within a given range
    int wrap(int n, int change, int min, int max);

    /// Converts numerical values to strings using stringstream
    string ToString(float n);
    string ToString(int n);

}

#endif // BASICS_H
