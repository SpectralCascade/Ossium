#include <cmath>

#include "basics.h"
#include "vector.h"

using namespace std;

namespace Ossium
{

    inline namespace structs
    {

        float Vector::Dot(Vector vec)
        {
            return (x * vec.x) + (y * vec.y);
        }

        float Vector::MagnitudeSquared()
        {
            return (x * x) + (y * y);
        }

        float Vector::Magnitude()
        {
            return sqrt(MagnitudeSquared());
        }

        Vector Vector::Normalised()
        {
            if (x == 0.0f && y == 0.0f)
            {
                return *this;
            }
            return (*this) * (1.0f / Magnitude());
        }

        Vector Vector::ProjectOnto(Vector vec)
        {
            return vec * (Dot(vec) / vec.MagnitudeSquared());
        }

        float Vector::DistanceSquared(Vector point)
        {
            point -= (*this);
            return point.MagnitudeSquared();
        }

        float Vector::Distance(Vector point)
        {
            point -= (*this);
            return point.Magnitude();
        }

        Vector Vector::Reflection(Vector normal)
        {
            return (*this) - (2.0f * ProjectOnto(normal));
        }

        Vector Vector::Rotation90Clockwise()
        {
            Vector output;
            output.x = y;
            output.y = -x;
            return output;
        }

        void Vector::Rotate90Clockwise()
        {
            float cachex = x;
            x = y;
            y = -cachex;
        }

        Vector Vector::Lerp(Vector vec, float w)
        {
            w = clamp(w, 0.0f, 1.0f);
            return ((*this) * (1.0f - w)) + (vec * w);
        }

        void Vector::Rotate(float degrees)
        {
            RotateRad((constants::pi / 180.0f) * degrees);
        }

        void Vector::RotateRad(float radians)
        {
            float length = Magnitude();
            x = SDL_sinf(radians) * length;
            y = SDL_cosf(radians) * length;
        }

        string Vector::ToString()
        {
            return "(" + functions::ToString(x) + ", " + functions::ToString(y) + ")";
        }

        void Vector::FromString(string str)
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

    Vector operator+(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    void operator+=(Vector &vec_a, const Vector& vec_b)
    {
        vec_a = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    }

    Vector operator-(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    void operator-=(Vector& vec_a, const Vector& vec_b)
    {
        vec_a = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    }

    Vector operator*(const Vector& vec_a, const Vector& vec_b)
    {
        return {vec_a.x * vec_b.x, vec_a.y * vec_b.y};
    }

    Vector operator*(const Vector& vec, float scalar)
    {
        return {vec.x * scalar, vec.y * scalar};
    }
    Vector operator*(float scalar, const Vector& vec)
    {
        return {vec.x * scalar, vec.y * scalar};
    }

}
