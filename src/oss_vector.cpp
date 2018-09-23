#include <math.h>

#include "oss_vector.h"

using namespace std;

OSS_Vector operator+(OSS_Vector vec_a, OSS_Vector vec_b)
{
    return {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
}

void operator+=(OSS_Vector &vec_a, OSS_Vector vec_b)
{
    vec_a = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
}

OSS_Vector operator-(OSS_Vector vec_a, OSS_Vector vec_b)
{
    return {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
}

void operator-=(OSS_Vector &vec_a, OSS_Vector vec_b)
{
    vec_a = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
}

OSS_Vector operator*(OSS_Vector vec_a, OSS_Vector vec_b)
{
    return {vec_a.x * vec_b.x, vec_a.y * vec_b.y};
}

OSS_Vector operator*(OSS_Vector vec, float scalar)
{
    return {vec.x * scalar, vec.y * scalar};
}
OSS_Vector operator*(float scalar, OSS_Vector vec)
{
    return {vec.x * scalar, vec.y * scalar};
}

float OSS_CalcDotProduct(OSS_Vector vec_a, OSS_Vector vec_b)
{
    return (vec_a.x * vec_b.x) + (vec_a.y * vec_b.y);
}

float OSS_CalcMagnitudeSquared(OSS_Vector vec)
{
    return (vec.x * vec.x) + (vec.y * vec.y);
}

float OSS_CalcMagnitude(OSS_Vector vec)
{
    return sqrt(OSS_CalcMagnitudeSquared(vec));
}

OSS_Vector OSS_CalcUnitVector(OSS_Vector vec)
{
    if (vec.x == 0.0 && vec.y == 0.0)
    {
        return vec;
    }
    return vec * (1.0 / OSS_CalcMagnitude(vec));
}

OSS_Vector OSS_CalcProjectVector(OSS_Vector vec_a, OSS_Vector vec_b)
{
    return vec_b * (OSS_CalcDotProduct(vec_a, vec_b) / OSS_CalcMagnitudeSquared(vec_b));
}

float OSS_CalcDistanceSquared(OSS_Vector pointA, OSS_Vector pointB)
{
    return OSS_CalcMagnitudeSquared(pointB - pointA);
}

float OSS_CalcDistance(OSS_Vector pointA, OSS_Vector pointB)
{
    return OSS_CalcMagnitude(pointB - pointA);
}

OSS_Vector OSS_CalcReflectionVector(OSS_Vector vec, OSS_Vector normal)
{
    return vec - (2 * OSS_CalcProjectVector(vec, normal));
}

OSS_Vector OSS_CalcRotation90Clockwise(OSS_Vector vec)
{
    return {vec.y, -vec.x};
}

void OSS_CalcRotation90Clockwise(OSS_Vector &vec)
{
    vec = {vec.y, -vec.x};
}
