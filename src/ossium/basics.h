#ifndef BASICS_H
#define BASICS_H

#include <string>
#include <sstream>

using namespace std;

namespace ossium
{
    /// Clamps a number to maxima/minima
    float clamp(float n, float min, float max);
    int clamp(int n, int min, int max);

    /// Wraps an integer number within a given range
    int wrap(int n, int change, int min, int max);

    /// Converts numerical values to strings using stringstream
    string ToString(float n);
    string ToString(int n);

    /// Removes spaces from either end of an input string and returns a new string
    string strip(string data);

    /// Determine whether a stripped ASCII string is an integer
    bool IsInt(const string& data);
    /// Determine whether a stripped ASCII string is a float
    bool IsFloat(const string& data);
    /// Convenient method to check whether a string is numerical at all
    bool IsNumber(const string& data);

    /// Convert a string to int or float using stringstream
    int ToInt(const string& data);
    float ToFloat(const string& data);

#define STRINGIFY(CODE) "CODE"

}

#endif // BASICS_H
