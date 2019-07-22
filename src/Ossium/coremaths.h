#ifndef VECTOR_H
#define VECTOR_H

#include <SDL.h>
#include <string>
#include <Box2D/Common/b2Math.h>

#include "mathconsts.h"

using namespace std;

namespace Ossium
{

    inline namespace Structs
    {

        struct Vector2 : public b2Vec2
        {
            Vector2(float _x = 0.0f, float _y = 0.0f);

            /// Dot product of this and another vector
            float Dot(Vector2 vec);

            /// Get a normalized version of this vector.
            Vector2 Normalized();

            /// Vector2 projection of vec_a onto vec_b
            Vector2 ProjectOnto(Vector2 vec);

            /// Calculate distance squared between two point vectors
            float DistanceSquared(Vector2 point);

            /// Calculate distance between two point vectors
            float Distance(Vector2 point);

            /// Calculate reflection of vector
            Vector2 Reflection(Vector2 normal);

            /// Rotate a vector 90 degrees clockwise
            Vector2 Rotation90Clockwise();
            /// Rotate this vector 90 degrees clockwise
            void Rotate90Clockwise();

            /// Rotate a vector 90 degrees clockwise
            Vector2 Rotation90AntiClockwise();
            /// Rotate this vector 90 degrees clockwise
            void Rotate90AntiClockwise();

            /// Calculate some point between two vectors (LERP - linear interpolation)
            Vector2 Lerp(Vector2 vec, float w);

            /// Gets the rotation of this vector in radians
            inline float RotationRad()
            {
                return SDL_atan2(x, y);
            }
            /// Gets the rotation of this vector in degrees
            inline float Rotation()
            {
                return RotationRad() * (180.0f / Constants::pi);
            }
            /// Rotates the vector in degrees
            void Rotate(float degrees);
            /// Rotates the vector in radians
            void RotateRad(float radians);
            /// Returns a vector rotated by degrees
            Vector2 Rotation(float degrees);
            /// Returns a vector rotated by radians
            Vector2 RotationRad(float radians);

            /// String conversion methods
            string ToString();
            void FromString(string str);

        };

    }

    /// Basic vector arithmetic
    Vector2 operator+(const Vector2& vec_a, const Vector2& vec_b);

    void operator+=(Vector2 &vec_a, const Vector2& vec_b);

    Vector2 operator-(const Vector2& vec_a, const Vector2& vec_b);

    void operator-=(Vector2 &vec_a, const Vector2& vec_b);

    Vector2 operator*(const Vector2& vec_a, const Vector2& vec_b);

    /// Scalar multiplication
    inline Vector2 operator*(const Vector2& vec, float scalar)
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }
    inline Vector2 operator*(float scalar, const Vector2& vec)
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }

}

#endif // VECTOR_H
