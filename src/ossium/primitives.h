#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL2/SDL.h>

#include "vector.h"

namespace ossium
{
    struct Circle
    {
        float x;
        float y;
        float r;
    };

    struct Line
    {
        Vector p;
        Vector u;
    };

    struct LineSegment
    {
        Vector a;
        Vector b;
    };

    struct Ray
    {
        Vector p;
        Vector u;
    };

    /// Ossium alternative to SDL_Rect; floating point instead of integers
    struct Rectangle
    {
        float x;
        float y;
        float w;
        float h;
        inline float xmax()
        {
            return x + w;
        };
        inline float ymax()
        {
            return y + h;
        };
    };

    struct Triangle
    {
        Vector a;
        Vector b;
        Vector c;
    };

    /// Now for the fun part... intersection tests
    /// There is some duplication here, but it should make auto-complete nicer
    /// Point intersection tests
    bool Intersect(Vector point, Circle circle);
    bool Intersect(Vector point, Line line);
    bool Intersect(Vector point, LineSegment segment);
    bool Intersect(Vector point, Ray ray);
    bool Intersect(Vector point, Rectangle rect);
    /*
    bool Intersect(Vector point, Triangle triangle);

    /// Circle intersection tests
    bool Intersect(Circle circleA, Circle circleB);
    bool Intersect(Circle circle, Line line);
    bool Intersect(Circle circle, LineSegment segment);
    bool Intersect(Circle circle, Ray ray);
    bool Intersect(Circle circle, Rectangle rect);
    bool Intersect(Circle circle, Triangle triangle);

    /// Line intersection tests
    bool Intersect(Line lineA, Line lineB);
    bool Intersect(Line line, Circle circle);
    bool Intersect(Line line, LineSegment segment);
    bool Intersect(Line line, Ray ray);
    bool Intersect(Line line, Rectangle rect);
    bool Intersect(Line line, Triangle triangle);

    /// Line segment intersection tests
    bool Intersect(LineSegment segmentA, LineSegment segmentB);
    bool Intersect(LineSegment segment, Circle circle);
    bool Intersect(LineSegment segment, Line line);
    bool Intersect(LineSegment segment, Ray ray);
    bool Intersect(LineSegment segment, Rectangle rect);
    bool Intersect(LineSegment segment, Triangle triangle);

    /// Ray intersection tests
    bool Intersect(Ray rayA, Ray rayB);
    bool Intersect(Ray ray, Circle circle);
    bool Intersect(Ray ray, Line line);
    bool Intersect(Ray ray, LineSegment segment);
    bool Intersect(Ray ray, Rectangle rect);
    bool Intersect(Ray ray, Triangle triangle);
*/
    /// Rectangle intersection tests, AABB only
    bool Intersect(Rectangle rectA, Rectangle rectB);
/*
    bool Intersect(Rectangle rect, Circle circle);
    bool Intersect(Rectangle rect, Line line);
    bool Intersect(Rectangle rect, LineSegment segment);
    bool Intersect(Rectangle rect, Ray ray);
    bool Intersect(Rectangle rect, Triangle triangle);

    /// Triangle intersection tests
    bool Intersect(Triangle triangleA, Triangle triangleB);
    bool Intersect(Triangle triangle, Circle circle);
    bool Intersect(Triangle triangle, Line line);
    bool Intersect(Triangle triangle, LineSegment segment);
    bool Intersect(Triangle triangle, Ray ray);
    bool Intersect(Triangle triangle, Rectangle rect);
    */

    /// SDL intersection tests
    bool IntersectSDL(SDL_Rect rectA, SDL_Rect rectB);
    bool IntersectSDL(SDL_Point point, SDL_Rect rect);

}

#endif // PRIMITIVES_H
