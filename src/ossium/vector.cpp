#include <math.h>

#include "basics.h"
#include "vector.h"

using namespace std;

namespace Ossium
{
    Vector operator+(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    void operator+=(Vector &vec_a, const Vector& vec_b)
    {
        vec_a = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    Vector operator-(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    void operator-=(Vector& vec_a, const Vector& vec_b)
    {
        vec_a = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    Vector operator*(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x * vec_b.x, vec_a.y * vec_b.y};
    }

    Vector operator*(const Vector& vec, float scalar)
    {
        return {vec.x * scalar, vec.y * scalar};
    }
    Vector operator*(float scalar, const Vector& vec)
    {
        return {vec.x * scalar, vec.y * scalar};
    }

    float CalcDotProduct(Vector vec_a, Vector vec_b)
    {
        return (vec_a.x * vec_b.x) + (vec_a.y * vec_b.y);
    }

    float CalcMagnitudeSquared(Vector vec)
    {
        return (vec.x * vec.x) + (vec.y * vec.y);
    }

    float CalcMagnitude(Vector vec)
    {
        return sqrt(CalcMagnitudeSquared(vec));
    }

    Vector CalcUnitVector(Vector vec)
    {
        if (vec.x == 0.0 && vec.y == 0.0)
        {
            return vec;
        }
        return vec * (1.0 / CalcMagnitude(vec));
    }

    Vector CalcProjectVector(Vector vec_a, Vector vec_b)
    {
        return vec_b * (CalcDotProduct(vec_a, vec_b) / CalcMagnitudeSquared(vec_b));
    }

    float CalcDistanceSquared(Vector pointA, Vector pointB)
    {
        return CalcMagnitudeSquared(pointB - pointA);
    }

    float CalcDistance(Vector pointA, Vector pointB)
    {
        return CalcMagnitude(pointB - pointA);
    }

    Vector CalcReflectionVector(Vector vec, Vector normal)
    {
        return vec - (2 * CalcProjectVector(vec, normal));
    }

    Vector CalcRotation90Clockwise(Vector vec)
    {
        return {vec.y, -vec.x};
    }

    void CalcRotation90Clockwise(Vector &vec)
    {
        vec = {vec.y, -vec.x};
    }

    Vector CalcLerp(Vector a, Vector b, float w)
    {
        w = clamp(w, 0.0f, 1.0f);
        return (a * (1.0f - w)) + (b * w);
    }

}
