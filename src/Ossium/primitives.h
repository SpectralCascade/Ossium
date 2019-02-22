#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL.h>
#include <vector>

#include "vector.h"
#include "renderer.h"

using namespace std;

namespace Ossium
{

    inline namespace structs
    {

        /// Forward declarations
        struct Circle;
        struct Point;
        struct Line;
        struct Ray;
        struct InfiniteLine;
        struct Triangle;
        struct Polygon;
        struct Rect;

        struct Circle
        {
            float x;
            float y;
            float r;
        };

        struct Point : public Vector
        {
            Point();
            Point(const Vector& vec);

            void Draw(Renderer& renderer);
            void Draw(Renderer& renderer, SDL_Color colour);

            /// Calculates a point on a line (where this is the start and p is the end of the line). A.K.A. linear interpolation
            Point Lerp(Point p, float w);

            /// Calculates the squared distance to another point
            float DistanceSquared(Point p);

            /// Calculate the distance to another point
            float Distance(Point p);

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

        struct Ray
        {
            Point p;
            Vector u;
        };

        struct InfiniteLine : public Ray
        {
        };

        struct Line
        {
            Point a;
            Point b;

            void Draw(Renderer& renderer);
            void Draw(Renderer& renderer, SDL_Color colour);

        };

        /// Floating point rectangle; if you want an integer based rectangle, use SDL_Rect instead
        struct Rect
        {
            float x;
            float y;
            float w;
            float h;

            void Draw(Renderer& renderer);
            void Draw(Renderer& renderer, SDL_Color colour);

            void DrawFilled(Renderer& renderer);
            void DrawFilled(Renderer& renderer, SDL_Color colour);

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
            void Draw(Renderer& renderer, SDL_Color colour);

            void DrawFilled(Renderer& renderer);
            void DrawFilled(Renderer& renderer, SDL_Color colour);

        };

        /// Represents a general shape
        struct Polygon
        {
            /// Renders the edges of the polygon
            void Draw(Renderer& renderer);

            /// Renders the polygon in a solid colour
            void DrawFilled(Renderer& renderer);

            void Draw(Renderer& renderer, SDL_Color colour);
            void DrawFilled(Renderer& renderer, SDL_Color colour);

            /// The vertices of the polygon
            vector<Point> vertices;

        };

    }

    /*
     *    TODO: put these intersection tests inside their respective structs/classes and actually implement them!
     */
    /*
    bool Intersect(Point point, Triangle triangle);

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
    bool Intersect(Rect rectA, Rect rectB);
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

}

#endif // PRIMITIVES_H
