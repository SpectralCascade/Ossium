#include <cmath>

#include "primitives.h"

namespace Ossium
{

    inline namespace structs
    {

        ///
        /// Circle
        ///

        bool Circle::Intersects(Circle circle)
        {
            float totalRadius = (r * circle.r);
            return Point(x, y).DistanceSquared(Point(circle.x, circle.y)) < totalRadius * totalRadius;
        }
        bool Circle::Intersects(InfiniteLine infiniteLine)
        {
            /// TODO
            return false;
        }
        bool Circle::Intersects(Line line)
        {
            /// TODO
            return false;
        }
        bool Circle::Intersects(Ray ray)
        {
            /// TODO
            return false;
        }
        bool Circle::Intersects(Rect rect)
        {
            Point offset;
            if (x < rect.x)
            {
                offset.x = rect.x;
            }
            else if (x > rect.xmax())
            {
                offset.x = rect.xmax();
            }
            else
            {
                offset.x = x;
            }
            if (y < rect.y)
            {
                offset.y = rect.y;
            }
            else if (y > rect.ymax())
            {
                offset.y = rect.ymax();
            }
            else
            {
                offset.y = y;
            }
            return (Point(x, y).DistanceSquared(offset) < r * r);
        }
        bool Circle::Contains(Point point)
        {
            return point.Intersects(*this);
        }

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
            SDL_RenderDrawPoint(renderer.GetRendererSDL(), round(x), round(y));
        }

        void Point::Draw(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
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
            return Intersects(infLine) && line.a.DistanceSquared(*this) <= line.a.DistanceSquared(line.b);
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

        Line::Line(Point start, Point end)
        {
            a = start;
            b = end;
        }

        void Line::Draw(Renderer& renderer)
        {
            SDL_RenderDrawLine(renderer.GetRendererSDL(), round(a.x), round(a.y), round(b.x), round(b.y));
        }

        void Line::Draw(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
            Draw(renderer);
        }

        ///
        /// Rect
        ///

        Rect::Rect()
        {
        }

        Rect::Rect(float xpos, float ypos, float width, float height)
        {
            x = xpos;
            y = ypos;
            w = width;
            h = height;
        }

        void Rect::DrawFilled(Renderer& renderer)
        {
            SDL_Rect rect = SDL();
            SDL_RenderFillRect(renderer.GetRendererSDL(), &rect);
        }

        void Rect::DrawFilled(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
            DrawFilled(renderer);
        }

        void Rect::Draw(Renderer& renderer)
        {
            SDL_Rect rect = SDL();
            SDL_RenderDrawRect(renderer.GetRendererSDL(), &rect);
        }

        void Rect::Draw(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
            Draw(renderer);
        }

        bool Rect::Intersects(Circle circle)
        {
            return Point(x, y).Intersects(circle) || Point(x, ymax()).Intersects(circle) || Point(xmax(), y).Intersects(circle) || Point(xmax(), ymax()).Intersects(circle);
        }
        bool Rect::Intersects(InfiniteLine infiniteLine)
        {
            /// TODO
            return false;
        }
        bool Rect::Intersects(Line line)
        {
            /// TODO
            return false;
        }
        bool Rect::Intersects(Ray ray)
        {
            /// TODO
            return false;
        }
        bool Rect::Intersects(Rect rect)
        {
            return !((x > rect.xmax() || xmax() < rect.x) && (y > rect.ymax() || ymax() < rect.y));
        }
        bool Rect::Contains(Point point)
        {
            return point.Intersects(*this);
        }

        SDL_Rect Rect::SDL()
        {
            return (SDL_Rect){(int)round(x), (int)round(y), (int)round(w), (int)round(h)};
        }

        ///
        /// Polygon
        ///

        void Polygon::DrawFilled(Renderer& renderer)
        {
            /// TODO: this
        }

        void Polygon::DrawFilled(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
            DrawFilled(renderer);
        }

        void Polygon::Draw(Renderer& renderer, SDL_Color color)
        {
            renderer.SetDrawColor(color);
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

}
