#ifndef VECTOR_H
#define VECTOR_H

namespace Ossium
{

    inline namespace structs
    {

        struct Vector
        {
            float x;
            float y;
        };

    }

    /// Basic vector arithmetic
    Vector operator+(const Vector& vec_a, const Vector& vec_b);

    void operator+=(Vector &vec_a, const Vector& vec_b);

    Vector operator-(const Vector& vec_a, const Vector& vec_b);

    void operator-=(Vector &vec_a, const Vector& vec_b);

    Vector operator*(const Vector& vec_a, const Vector& vec_b);

    /// Scalar multiplication
    Vector operator*(const Vector& vec, float scalar);
    Vector operator*(float scalar, const Vector& vec);

    /// Dot product
    float CalcDotProduct(Vector vec_a, Vector vec_b);

    /// Magnitude of vector squared
    float CalcMagnitudeSquared(Vector vec);

    /// Magnitude of vector
    float CalcMagnitude(Vector vec);

    /// Normalised (unit) vector
    Vector CalcUnitVector(Vector vec);

    /// Vector projection of vec_a onto vec_b
    Vector CalcProjectVector(Vector vec_a, Vector vec_b);

    /// Calculate distance squared between two point vectors
    float CalcDistanceSquared(Vector pointA, Vector pointB);

    /// Calculate distance between two point vectors
    float CalcDistance(Vector pointA, Vector pointB);

    /// Calculate reflection of vector
    Vector CalcReflectionVector(Vector vec, Vector normal);

    /// Rotate a vector 90 degrees clockwise (anti-clockwise in SDL coordinate system)...
    Vector CalcRotation90Clockwise(Vector vec);
    void CalcRotation90Clockwise(Vector &vec);

    /// Calculate some point between two vectors (LERP - linear interpolation)
    Vector CalcLerp(Vector a, Vector b, float w);
}

#endif // VECTOR_H
