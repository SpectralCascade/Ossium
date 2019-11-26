#ifndef VECTOR_H
#define VECTOR_H

#include <string>
#include <Box2D/Common/b2Math.h>

#include "mathconsts.h"
#include "renderer.h"
#include "helpermacros.h"

using namespace std;

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

    struct Vector2 : public b2Vec2
    {
        Vector2() = default;
        Vector2(float _x, float _y);
        Vector2(b2Vec2& vec) { *this = vec; };

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

    struct Rotation : public b2Rot
    {
        Rotation() = default;
        Rotation(float angle) : b2Rot(angle) {};
        Rotation(Vector2& direction) : b2Rot(direction.RotationRad()) {};

        /// Returns the angle in degrees.
        float GetDegrees();
        /// Returns the angle in radians.
        float GetRadians();
        /// Sets the angle in degrees.
        void SetDegrees(float degrees);
        /// Sets the angle in radians.
        void SetRadians(float radians);

        /// Convert rotation from a given string
        void FromString(string data);
        /// Convert rotation to a string
        string ToString();

    protected:
        using b2Rot::Set;
        using b2Rot::GetAngle;

    };

    /// Forward declarations
    struct Circle;
    struct Ray;
    struct Line;
    struct InfiniteLine;
    struct Triangle;
    struct Polygon;
    struct Rect;

    /// Similar to Vector2, but also implements drawing methods.
    struct Point : public Vector2
    {
        Point() = default;
        Point(float _x, float _y);
        Point(const Vector2& vec);

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

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

    struct Transform : public b2Transform
    {
        Transform() = default;
        Transform(const Vector2& position, const Rotation& rotation) : b2Transform((b2Vec2)position, (b2Rot)rotation) {};

        /// Typically using EVIL_CAST would NOT be acceptable,
        /// but for this specific use case I deem it acceptable as the Point class
        /// not only inherits from Vector2 (which in turn inherits from b2Vec2),
        /// but Point is the same size as b2Vec2 and ALWAYS WILL BE. Of course,
        /// the argument against this would be that "What if someone wants to add more
        /// members to Point or Vector2 (or make one of them virtual)!?". To which I answer:
        /// the person who does that doesn't understand the purpose of the Point struct
        /// and shouldn't be touching the code in such a case. If they really want to
        /// add some members, they should just inherit from Point and do their own thing.
        inline Point& position() { return EVIL_CAST(p, Point); };
        /// Ditto for the Rotation casting code.
        inline Rotation& rotation() { return EVIL_CAST(q, Rotation); };

        /// Convert from string
        void FromString(string data);
        /// Convert to string
        string ToString();

    protected:
        using b2Transform::p;
        using b2Transform::q;

    };

    struct Circle
    {
        float x;
        float y;
        float r;

        void Draw(Renderer& renderer, float smoothness = 1.0f);
        void Draw(Renderer& renderer, SDL_Color color, float smoothness = 1.0f);

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

    struct Ray
    {
        Ray() = default;
        Ray(Point origin, Vector2 direction);

        Point p;
        Vector2 u;
    };

    struct InfiniteLine : public Ray
    {
        InfiniteLine() = default;
        InfiniteLine(Point p, Vector2 direction);
    };

    struct Line
    {
        Line() = default;
        Line(Point start, Point end);

        Point a;
        Point b;

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

    };

    /// Floating point rectangle; if you want an integer based rectangle, use SDL_Rect instead
    struct Rect
    {
        Rect();
        Rect(float xpos, float ypos, float width, float height);

        float x;
        float y;
        float w;
        float h;

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

        void DrawFilled(Renderer& renderer);
        void DrawFilled(Renderer& renderer, SDL_Color color);

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

        /// Returns the  SDL_Rect equivalent of this rect for convenience
        SDL_Rect SDL();

    };

    struct Triangle
    {
        Point a;
        Point b;
        Point c;

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

        void DrawFilled(Renderer& renderer);
        void DrawFilled(Renderer& renderer, SDL_Color color);

    };

    /// Represents a general shape
    struct Polygon
    {
        /// Renders the edges of the polygon
        void Draw(Renderer& renderer);

        /// Renders the polygon in a solid color
        void DrawFilled(Renderer& renderer);

        void Draw(Renderer& renderer, SDL_Color color);
        void DrawFilled(Renderer& renderer, SDL_Color color);

        /// The vertices of the polygon
        vector<Point> vertices;

    };

    ///
    /// Vector2 maths
    ///

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
