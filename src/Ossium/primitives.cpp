#include <cmath>

#include "primitives.h"

namespace Ossium
{

    inline namespace structs
    {

        /// Note: Everything does integer truncation when rendering. Should rounding be used instead?

        ///
        /// Point
        ///

        Point::Point()
        {
        }

        Point::Point(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        Point::Point(const Vector& vec)
        {
            x = vec.x;
            y = vec.y;
        }

        void Point::Draw(Renderer& renderer)
        {
            SDL_RenderDrawPoint(renderer.GetRendererSDL(), x, y);
        }

        void Point::Draw(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            Draw(renderer);
        }

        Point Point::Lerp(Point p, float w)
        {
            return static_cast<Point>(Vector::Lerp(static_cast<Vector>(p), w));
        }

        float Point::DistanceSquared(Point p)
        {
            return Vector::DistanceSquared(static_cast<Vector>(p));
        }

        float Point::Distance(Point p)
        {
            return Vector::Distance(static_cast<Vector>(p));
        }

        bool Point::Intersects(Circle circle)
        {
            return DistanceSquared((Vector){circle.x, circle.y}) <= circle.r * circle.r;
        }
        bool Point::Intersects(InfiniteLine infiniteLine)
        {
            /// Treat this as a point on the line and check if it has the same Y-intercept
            float m = (infiniteLine.u.y / infiniteLine.u.x);
            return (y - (m * x)) == (infiniteLine.p.y - (m * infiniteLine.p.x));
        }
        bool Point::Intersects(Line line)
        {
            /// Similar to above, but within a limited range
            InfiniteLine infLine = {line.a, (Vector)line.b};
            return Intersects(infLine) &&
             line.a.DistanceSquared(*this) <= line.a.DistanceSquared(line.b);
        }
        bool Point::Intersects(Ray ray)
        {
            /// Same as infinite line but we also check if the point is on the correct side
            InfiniteLine line = {ray.p, static_cast<Point>(ray.u)};
            return Intersects(line) && ray.u.Dot(*this) > 0.0;
        }
        bool Point::Intersects(Rect rect)
        {
            return x >= rect.x && x <= rect.xmax() && y >= rect.y && y <= rect.ymax();
        }

        ///
        /// Line
        ///

        void Line::Draw(Renderer& renderer)
        {
            SDL_RenderDrawLine(renderer.GetRendererSDL(), a.x, a.y, b.x, b.y);
        }

        void Line::Draw(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            Draw(renderer);
        }

        void Rect::DrawFilled(Renderer& renderer)
        {
            SDL_Rect rect = SDL();
            SDL_RenderFillRect(renderer.GetRendererSDL(), &rect);
        }

        void Rect::DrawFilled(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            DrawFilled(renderer);
        }

        void Rect::Draw(Renderer& renderer)
        {
            SDL_Rect rect = SDL();
            SDL_RenderDrawRect(renderer.GetRendererSDL(), &rect);
        }

        void Rect::Draw(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            Draw(renderer);
        }

        SDL_Rect Rect::SDL()
        {
            return (SDL_Rect){(int)round(x), (int)round(y), (int)round(w), (int)round(h)};
        }

        void Polygon::DrawFilled(Renderer& renderer)
        {
            /// TODO: this
        }

        void Polygon::DrawFilled(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            DrawFilled(renderer);
        }

        void Polygon::Draw(Renderer& renderer, SDL_Color colour)
        {
            renderer.SetDrawColour(colour);
            Draw(renderer);
        }

        void Polygon::Draw(Renderer& renderer)
        {
            if (!vertices.empty())
            {
                Point previousPoint = vertices[0];
                for (unsigned int i = 1, counti = vertices.size(); i < counti; i++)
                {
                    SDL_RenderDrawLine(renderer.GetRendererSDL(), (int)previousPoint.x, (int)previousPoint.y, (int)vertices[i].x, (int)vertices[i].y);
                    previousPoint = vertices[i];
                }
                SDL_RenderDrawLine(renderer.GetRendererSDL(), (int)previousPoint.x, (int)previousPoint.y, (int)vertices[0].x, (int)vertices[0].y);
            }
        }

    }

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
    bool Intersect(Rect rectA, Rect rectB)
    {
        return !((rectA.x > rectB.xmax() || rectA.xmax() < rectB.x) && (rectA.y > rectB.ymax() || rectA.ymax() < rectB.y));
    }
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

    /// Intersection tests for basic SDL data types
    bool IntersectSDL(SDL_Rect rectA, SDL_Rect rectB)
    {
        return !((rectA.x > rectB.x + rectB.w || rectA.x + rectA.w < rectB.x) && (rectA.y > rectB.y + rectB.h || rectA.y + rectA.h < rectB.y));
    }
    bool IntersectSDL(SDL_Point point, SDL_Rect rect)
    {
        return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
    }

}
