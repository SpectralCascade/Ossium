#ifndef VECTOR_H
#define VECTOR_H

#include <SDL.h>

namespace Ossium
{

    namespace constants
    {
        const float pi = 3.14159265358979f;
    }

    inline namespace structs
    {

        struct Vector
        {
            float x;
            float y;

            /// Dot product of this and another vector
            float Dot(Vector vec);

            /// Magnitude of this vector squared
            inline float MagnitudeSquared();

            /// Magnitude of this vector
            float Magnitude();

            /// Normalised (unit) vector
            Vector Normalised();

            /// Vector projection of vec_a onto vec_b
            Vector ProjectOnto(Vector vec);

            /// Calculate distance squared between two point vectors
            float DistanceSquared(Vector point);

            /// Calculate distance between two point vectors
            float Distance(Vector point);

            /// Calculate reflection of vector
            Vector Reflection(Vector normal);

            /// Rotate a vector 90 degrees clockwise
            Vector Rotation90Clockwise();
            /// Rotate this vector 90 degrees clockwise
            void Rotate90Clockwise();

            /// Calculate some point between two vectors (LERP - linear interpolation)
            Vector Lerp(Vector vec, float w);

            /// Gets the rotation of this vector in radians
            inline float RotationRad()
            {
                return SDL_atan2(x, y);
            }
            /// Gets the rotation of this vector in degrees
            inline float Rotation()
            {
                return RotationRad() * (180.0f / constants::pi);
            }
            /// Rotates the vector in degrees
            void Rotate(float degrees);
            /// Rotates the vector in radians
            void RotateRad(float radians);

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

}

#endif // VECTOR_H
