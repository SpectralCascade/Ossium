/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#ifndef VECTOR_H
#define VECTOR_H

#include <string>
#include <type_traits>
#include <initializer_list>

#include "mathconsts.h"
#include "renderer.h"
#include "helpermacros.h"
#include "matrix.h"

namespace Ossium
{

    inline float RadToDeg(float angle)
    {
        return (180.0f / Constants::pi) * angle;
    }
    inline float DegToRad(float angle)
    {
        return (Constants::pi / 180.0f) * angle;
    }

    struct Vector3;

    // Represents a 2D vector.
    // TODO move many of these methods into Matrix as template specialisations if possible
    struct Vector2 : public Matrix<2, 1>
    {
        Vector2() = default;
        Vector2(float x, float y) : Matrix({{x, y}}) {}
        Vector2(const Matrix<2, 1>& matrix) : Matrix(matrix) {}
        Vector2(const Matrix<3, 1>& matrix) : Matrix({{matrix.x, matrix.y}}) {}
        Vector2(const Vector3& vec);

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

        /// Calculate the length of this vector squared
        float LengthSquared();

        /// Calculate the length of this vector
        float Length();
        
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
        /// Returns a new vector based on this vector, rotated in degrees
        Vector2 Rotation(float degrees);
        /// Returns a new vector based on this vector, rotated in radians
        Vector2 RotationRad(float radians);

        /// Returns a new vector with the biggest of each vector's components.
        Vector2 Max(Vector2 vec);

        /// Returns a new vector with the smallest of each vector's components.
        Vector2 Min(Vector2 vec);

        /// String conversion methods
        std::string ToString();
        void FromString(const std::string& str);

        // Get the result of scaling this vector
        Vector2 operator*(float scalar);
        // Get the result of adding this vector to another vector
        Vector2 operator+(Vector2 operand);
        // Get the result of subtracting another vector from this vector
        Vector2 operator-(Vector2 operand);

        const static Vector2 Zero;
        const static Vector2 OneOne;
        const static Vector2 OneNegOne;
        const static Vector2 OneZero;
        const static Vector2 ZeroOne;
        const static Vector2 NegOneNegOne;
        const static Vector2 NegOneZero;
        const static Vector2 ZeroNegOne;

    };

    struct Vector3 : public Matrix<3, 1>
    {
        Vector3() = default;
        Vector3(float x, float y, float z);
        Vector3(const Matrix<3, 1>& matrix) : Matrix(matrix) {}
        Vector3(const Matrix<2, 1>& matrix) : Matrix(Matrix<3, 1>({{matrix.x, matrix.y, z}})) {}
        Vector3(const Vector2& vec) : Matrix(Matrix<3, 1>({{vec.x, vec.y, 0}})) {}
        Vector3(const MatrixBase<3, 1>& matrix) : Matrix(Matrix<3, 1>(matrix)) {}

        // Calculate the cross product of this vector with another vector
        Vector3 Cross(const Vector3& vec);

        // Calculate the square magnitude of this vector
        float LengthSquared();

        // Calculate the magnitude of this vector
        float Length();

        // Calculate the normalised form of this vector
        Vector3 Normalised();

        // Rotate this vector about the origin around the x axis (pitch), y axis (roll) and z axis (yaw) in degrees
        void Rotate(float pitch, float roll, float yaw);

        // Rotate this vector about the origin around the x axis (pitch), y axis (roll) and z axis (yaw) in radians
        void RotateRad(float pitch, float roll, float yaw);

        /// Returns a rotated vector (in degrees)
        Vector3 Rotation(float pitch, float roll, float yaw);

        /// Returns a rotated vector (in radians)
        Vector3 RotationRad(float pitch, float roll, float yaw);

        /// String conversion methods
        std::string ToString();
        void FromString(const std::string& str);

        const static Vector3 Zeroes;
        const static Vector3 Ones;
    };

    // Get the result of scaling this vector
    Vector2 operator*(float scalar, Vector2 operand);

    /*struct OSSIUM_EDL Rotation : public b2Rot
    {
        Rotation() = default;
        Rotation(const b2Rot& rot);
        Rotation(float angle) : b2Rot(angle) {};
        Rotation(Vector2&& direction) : b2Rot(direction.RotationRad()) {};

        /// Returns the angle in degrees.
        float GetDegrees();
        /// Returns the angle in radians.
        float GetRadians();
        /// Sets the angle in degrees.
        void SetDegrees(float degrees);
        /// Sets the angle in radians.
        void SetRadians(float radians);

        /// Convert rotation from a given string
        void FromString(const std::string& data);
        /// Convert rotation to a string
        std::string ToString();

    protected:
        using b2Rot::Set;
        using b2Rot::GetAngle;

    };*/

    /*inline Rotation operator*(const Rotation& rota, const Rotation& rotb)
    {
        return b2Mul(rota, rotb);
    }*/

    /// Forward declarations
    struct Circle;
    struct Ray;
    struct Line;
    struct InfiniteLine;
    struct Triangle;
    struct Polygon;
    struct Rect;

    /// Similar to Vector2, but also implements drawing methods.
    struct OSSIUM_EDL Point : public Vector2
    {
        Point() = default;
        Point(float _x, float _y);
        Point(const Vector2& vec);
        Point(const Matrix<2, 1>& mat) : Point(Vector2(mat)) {}

        void Draw(RenderInput* pass);
        void Draw(RenderInput* pass, SDL_Color color);

        /// Whether or not this point is intersecting a circle
        bool Intersects(Circle circle);
        /// Whether or not this point is intersecting an infinite line
        bool Intersects(InfiniteLine infiniteLine);
        /// Whether or not this point is intersecting a line
        bool Intersects(Line line);
        /// Whether or not this point is intersecting a ray
        bool Intersects(Ray ray);
        /// Whether or not this point is intersecting a rect
        bool Intersects(Rect rect);

    };

    struct OSSIUM_EDL Circle
    {
        float x;
        float y;
        float r;

        void Draw(RenderInput* pass, float smoothness = 1.0f);
        void Draw(RenderInput* pass, SDL_Color color, float smoothness = 1.0f);

        /// Whether or not this rect is intersecting a circle
        bool Intersects(Circle circle);
        /// Whether or not this rect is intersecting an infinite line
        bool Intersects(InfiniteLine infiniteLine);
        /// Whether or not this rect is intersecting a line
        bool Intersects(Line line);
        /// Whether or not this rect is intersecting a ray
        bool Intersects(Ray ray);
        /// Whether or not this rect is intersecting a rect
        bool Intersects(Rect rect);
        /// Whether or not a point is inside this rect
        bool Contains(Point point);
    };

    struct OSSIUM_EDL Ray
    {
        Ray() = default;
        Ray(Point origin, Vector2 direction);

        Point p;
        Vector2 u;
    };

    struct OSSIUM_EDL InfiniteLine : public Ray
    {
        InfiniteLine() = default;
        InfiniteLine(Point p, Vector2 direction);
    };

    struct OSSIUM_EDL Line
    {
        Line() = default;
        Line(Point start, Point end);

        Point a;
        Point b;

        void Draw(RenderInput* pass);
        void Draw(RenderInput* pass, SDL_Color color);

    };

    /// Floating point rectangle; if you want an integer based rectangle, use SDL_Rect instead
    struct OSSIUM_EDL Rect
    {
        Rect();
        Rect(float xpos, float ypos, float width, float height);
        Rect(const Vector2& position, const Vector2& dimensions);
        Rect(SDL_Rect rect);

        float x;
        float y;
        float w;
        float h;

        void Draw(RenderInput* pass);
        void Draw(RenderInput* pass, SDL_Color color);

        void DrawFilled(RenderInput* pass);
        void DrawFilled(RenderInput* pass, SDL_Color color);

        /// Whether or not this rect is intersecting a circle
        bool Intersects(Circle circle);
        /// Whether or not this rect is intersecting an infinite line
        bool Intersects(InfiniteLine infiniteLine);
        /// Whether or not this rect is intersecting a line
        bool Intersects(Line line);
        /// Whether or not this rect is intersecting a ray
        bool Intersects(Ray ray);
        /// Whether or not this rect is intersecting a rect
        bool Intersects(Rect rect);
        /// Whether or not a point is inside this rect
        bool Contains(Point point);

        inline int xmax()
        {
            return x + w;
        };
        inline int ymax()
        {
            return y + h;
        };

        bool operator==(const Rect& rect);
        bool operator!=(const Rect& rect);

        /// Returns the SDL_Rect equivalent of this rect for convenience
        SDL_Rect SDL();

        /// Converts a given string into a rect
        void FromString(const std::string& str);

        /// Returns the string representation of this object.
        std::string ToString();

        /// A rect initialised to zero.
        const static Rect Zero;

    };

    struct OSSIUM_EDL Triangle
    {
        Point a;
        Point b;
        Point c;

        void Draw(RenderInput* pass);
        void Draw(RenderInput* pass, SDL_Color color);

        void DrawFilled(RenderInput* pass);
        void DrawFilled(RenderInput* pass, SDL_Color color);

    };

    /// Represents a general shape
    struct OSSIUM_EDL Polygon
    {
        /// Renders the edges of the polygon
        void Draw(RenderInput* pass);

        /// Renders the polygon in a solid color
        void DrawFilled(RenderInput* pass);

        void Draw(RenderInput* pass, SDL_Color color);
        void DrawFilled(RenderInput* pass, SDL_Color color);

        /// The vertices of the polygon
        std::vector<Point> vertices;

    };

}

#endif // VECTOR_H
