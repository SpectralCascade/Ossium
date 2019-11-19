#include <cmath>

#include "funcutils.h"
#include "stringconvert.h"
#include "coremaths.h"

using namespace std;

namespace Ossium
{

    ///
    /// Vector2
    ///

    Vector2::Vector2(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    float Vector2::Dot(Vector2 vec)
    {
        return (x * vec.x) + (y * vec.y);
    }

    Vector2 Vector2::Normalized()
    {
        if (x == 0.0f && y == 0.0f)
        {
            return *this;
        }
        return (*this) * (1.0f / Length());
    }

    Vector2 Vector2::ProjectOnto(Vector2 vec)
    {
        return vec * (Dot(vec) / vec.LengthSquared());
    }

    float Vector2::DistanceSquared(Vector2 point)
    {
        point -= (*this);
        return point.LengthSquared();
    }

    float Vector2::Distance(Vector2 point)
    {
        point -= (*this);
        return point.Length();
    }

    Vector2 Vector2::Reflection(Vector2 normal)
    {
        return (*this) - (2.0f * ProjectOnto(normal));
    }

    Vector2 Vector2::Rotation90Clockwise()
    {
        Vector2 output;
        output.x = y;
        output.y = -x;
        return output;
    }

    void Vector2::Rotate90Clockwise()
    {
        float cachex = x;
        x = y;
        y = -cachex;
    }

    Vector2 Vector2::Rotation90AntiClockwise()
    {
        return Vector2(-y, x);
    }

    void Vector2::Rotate90AntiClockwise()
    {
        float cachex = x;
        x = -y;
        y = cachex;
    }

    Vector2 Vector2::Lerp(Vector2 vec, float w)
    {
        w = Clamp(w, 0.0f, 1.0f);
        return ((*this) * (1.0f - w)) + (vec * w);
    }

    void Vector2::Rotate(float degrees)
    {
        RotateRad((Constants::pi / 180.0f) * degrees);
    }

    void Vector2::RotateRad(float radians)
    {
        float length = Length();
        x = SDL_sinf(radians) * length;
        y = SDL_cosf(radians) * length;
    }

    Vector2 Vector2::Rotation(float degrees)
    {
        Vector2 v = *this;
        v.Rotate(degrees);
        return v;
    }

    Vector2 Vector2::RotationRad(float radians)
    {
        Vector2 v = *this;
        v.RotateRad(radians);
        return v;
    }

    string Vector2::ToString()
    {
        return "(" + Utilities::ToString(x) + ", " + Utilities::ToString(y) + ")";
    }

    void Vector2::FromString(string str)
    {
        unsigned int len = str.length();
        if (len > 4)
        {
            /// Remove brackets
            str = str.substr(1, len - 2);
            /// Split and get the individual values
            string xhalf = SplitLeft(str, ',');
            string yhalf = SplitRight(str, ',');
            x = ToFloat(xhalf);
            y = ToFloat(yhalf);
        }
    }

    ///
    /// Rotation
    ///

    float Rotation::GetDegrees()
    {
        return RadToDeg(GetAngle());
    }
    float Rotation::GetRadians()
    {
        return GetAngle();
    }
    void Rotation::SetDegrees(float angle)
    {
        Set(DegToRad(angle));
    }
    void Rotation::SetRadians(float angle)
    {
        Set(angle);
    }

    void Rotation::FromString(string data)
    {
        float degs;
        Utilities::FromString(degs, data);
        SetDegrees(degs);
    }

    string Rotation::ToString()
    {
        return Utilities::ToString(GetDegrees());
    }

    ///
    /// Point
    ///

    Point::Point(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    Point::Point(const Vector2& vec)
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

    bool Point::Intersects(Circle circle)
    {
        return DistanceSquared((Vector2){circle.x, circle.y}) <= circle.r * circle.r;
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
        InfiniteLine infLine = {line.a, (Vector2)line.b};
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
    /// Transform
    ///

    void Transform::FromString(string data)
    {
        Utilities::FromString(position().x, Utilities::SplitLeft(data, ',').substr(1));
        Utilities::FromString(position().y, Utilities::SplitLeft(Utilities::SplitRight(data, ',', ""), ','));
        Utilities::FromString(rotation(), Utilities::SplitLeft(Utilities::SplitRight(Utilities::SplitRight(data, ','), ','), ')'));
    }

    string Transform::ToString()
    {
        return "(" + Utilities::ToString(position().x) + ", " + Utilities::ToString(position().x) + ", " + Utilities::ToString(rotation()) + ")";
    }

    ///
    /// Ray
    ///

    Ray::Ray(Point origin, Vector2 direction)
    {
        p = origin;
        u = direction;
    }

    ///
    /// InfiniteLine
    ///

    InfiniteLine::InfiniteLine(Point p, Vector2 direction)
    {
        p = p;
        u = direction;
    }

    ///
    /// Circle
    ///

    /// TODO: use a more efficient drawing algorithm
    void Circle::Draw(Renderer& renderer, float smoothness)
    {
        int segments = r * r * Utilities::Clamp(smoothness);
        for (int i = 0; i < segments; i++)
        {
            SDL_RenderDrawLine(renderer.GetRendererSDL(),
                               x + r * sin(((Constants::pi * 2) / segments) * i), y + r * cos(((Constants::pi * 2) / segments) * i),
                               x + r * sin(((Constants::pi * 2) / segments) * (i + 1)), y + r * cos(((Constants::pi * 2) / segments) * (i + 1))
            );
        }
    }

    void Circle::Draw(Renderer& renderer, SDL_Color color, float smoothness)
    {
        renderer.SetDrawColor(color);
        Draw(renderer, smoothness);
    }

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

    Vector2 operator+(const Vector2& vec_a, const Vector2& vec_b)
    {
        return {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    void operator+=(Vector2 &vec_a, const Vector2& vec_b)
    {
        vec_a = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    Vector2 operator-(const Vector2& vec_a, const Vector2& vec_b)
    {
        return {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    void operator-=(Vector2& vec_a, const Vector2& vec_b)
    {
        vec_a = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    Vector2 operator*(const Vector2& vec_a, const Vector2& vec_b)
    {
        return {vec_a.x * vec_b.x, vec_a.y * vec_b.y};
    }

}
