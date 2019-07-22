#include <cmath>

#include "basics.h"
#include "coremaths.h"

using namespace std;

namespace Ossium
{

    inline namespace Structs
    {

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
            w = clamp(w, 0.0f, 1.0f);
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
                string xhalf = splitPairFirst(str, ',');
                string yhalf = splitPair(str, ',');
                x = ToFloat(xhalf);
                y = ToFloat(yhalf);
            }
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
