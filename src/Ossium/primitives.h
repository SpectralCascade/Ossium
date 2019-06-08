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

        struct Point : public Vector
        {
            Point();
            Point(float x, float y);
            Point(const Vector& vec);

            void Draw(Renderer& renderer);
            void Draw(Renderer& renderer, SDL_Color color);

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

    }

}

#endif // PRIMITIVES_H
