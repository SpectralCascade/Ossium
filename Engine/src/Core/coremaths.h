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

    template<unsigned int Dimensions, unsigned int Vectors, typename Enable = void>
    struct MatrixBase
    {
        // The underlying data, in row-major order (for contiguous memory layout).
        float data[Vectors][Dimensions];
    };

    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 2 && Vectors == 1>>
    {
        union {
            struct {
                float x;
                float y;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];    
        };
        
    };

    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 3 && Vectors == 1>>
    {
        union {
            struct {
                float x;
                float y;
                float z;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];
        };

        

    };

    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 2 && Vectors == 2>>
    {
        union {
            struct {
                float a;
                float c;
                float b;
                float d;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];
        };

        /// Returns the determinant of this matrix (signed volume of the parallelogram).
        float Determinant()
        {
            return a * d - b * c;
        }

    };

    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 3 && Vectors == 3>>
    {
        union {
            struct {
                float a;
                float d;
                float g;
                float b;
                float e;
                float h;
                float c;
                float f;
                float i;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];
        };

        /// Returns the determinant of this matrix (signed volume of the parallelpiped).
        float Determinant()
        {
            return (a * e * i) + (b * f * g) + (c * d * h) - (c * e * g) - (b * d * i) - (a * f * h);
        }

    };

    /// A matrix representation.
    /// I.e. data[0] = { x0, y0, z0 }, data[1] = { x1, y1, z1 } etc.
    /// such that each array row actually represents a column vector in the mathematical representation.
    /// For example, to create a 3x4 matrix: Matrix<3, 4>();
    template<unsigned int Dimensions, unsigned int Vectors>
    struct Matrix : public MatrixBase<Dimensions, Vectors>
    {
    private:
        // A matrix full of zeroes.
        static Matrix<Dimensions, Vectors> zeroes;
        static Matrix<Dimensions, Vectors> ones;

        typedef MatrixBase<Dimensions, Vectors> Base;

    public:
        inline const static unsigned int TotalVectors = Vectors;
        inline const static unsigned int TotalDimensions = Dimensions;

        Matrix() = default;

        Matrix(float init[Vectors][Dimensions])
        {
            memcpy(Base::data, init, Vectors * Dimensions);
        }

        Matrix(std::initializer_list<std::initializer_list<float>> init)
        {
            //Log.Debug("Creating matrix of size ({0}, {1})...", TotalDimensions, TotalVectors);
            DEBUG_ASSERT(
                init.size() == TotalVectors && init.begin()->size() == TotalDimensions,
                Utilities::Format(
                    "Matrix dimensions mismatch! Input dimensions = ({0}, {1}), dest dimensions = ({2}, {3})",
                    init.begin()->size(),
                    init.size(),
                    TotalDimensions,
                    TotalVectors
                )
            );
            unsigned int i = 0, j = 0;
            for (auto itr = init.begin(); itr != init.end(); itr++)
            {
                for (auto jtr = itr->begin(); jtr != itr->end(); jtr++)
                {
                    Base::data[i][j] = *jtr;
                    j++;
                }
                i++;
                j = 0;
            }
        }

        /// Returns the result of multiplying this with another matrix, such that if A = this, B = operand,
        /// it performs the multiplication: AB
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalVectors,
            Matrix<Dimensions, MatType::TotalVectors>
        >::type
        operator*(const MatType& operand)
        {
            // Make sure resultant matrix always starts initialised to zeroes.
            auto result = Matrix<Dimensions, MatType::TotalVectors>::Zeroes();

            // Iterate over each vector in the operand matrix.
            for (unsigned int operandVec = 0; operandVec < MatType::TotalVectors; operandVec++)
            {
                // Iterate over each element in that vector.
                for (unsigned int operandElement = 0; operandElement < MatType::TotalDimensions; operandElement++)
                {
                    // Finally, iterate over each element in the corresponding vector of this matrix.
                    for (unsigned int element = 0; element < Dimensions; element++)
                    {
                        // Due to row-major ordering, we specify column first THEN row when indexing.
                        result.data[operandVec][element] += 
                            Base::data[operandElement][element] * operand.data[operandVec][operandElement];
                    }
                }
            }
            return result;
        }

        /// Multiplies this with another matrix, such that if A = this, B = operand,
        /// it performs the multiplication: A = AB
        template<typename MatType>
        typename std::enable_if<MatType::TotalDimensions == TotalVectors, void>::type
        operator*=(const MatType& operand)
        {
            *this = (*this) * operand;
        }

        /// Returns the result of multiplying every element in this matrix by a scalar value.
        Matrix<TotalDimensions, TotalVectors> operator*(float scalar)
        {
            auto result = *this;
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                for (unsigned int j = 0; j < TotalDimensions; j++)
                {
                    result.data[i][j] *= scalar;
                }
            }
            return result;
        }

        /// Multiplies every element in this matrix by a scalar value.
        void operator*=(float scalar)
        {
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                for (unsigned int j = 0; j < TotalDimensions; j++)
                {
                    Base::data[i][j] *= scalar;
                }
            }
        }

        /// Returns the result of adding this with another matrix, such that if A = this, B = operand,
        /// it performs the entrywise sum: A + B
        /// TODO: Use const ref?
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            Matrix<Dimensions, MatType::TotalVectors>
        >::type
        operator+(MatType operand)
        {
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                for (unsigned int j = 0; j < TotalDimensions; j++)
                {
                    operand.data[i][j] += Base::data[i][j];
                }
            }
            return operand;
        }

        /// Returns the result of subtracting another matrix from this, such that if A = this, B = operand,
        /// it performs the entrywise subtraction: A - B
        /// TODO: Use const ref?
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            Matrix<Dimensions, MatType::TotalVectors>
        >::type
        operator-(MatType operand)
        {
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                for (unsigned int j = 0; j < TotalDimensions; j++)
                {
                    operand.data[i][j] = Base::data[i][j] - operand.data[i][j];
                }
            }
            return operand;
        }

        /// Sums another matrix with this and self-assigns the result, such that if A = this, B = operand,
        /// it performs the entrywise sum: A = A + B
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            void
        >::type
        operator+=(const MatType& operand)
        {
            *this = *this + operand;
        }

        /// Subtracts another matrix from this and self-assigns the result, such that if A = this,
        /// B = operand, it performs the entrywise sum: A = A - B
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            void
        >::type
        operator-=(const MatType& operand)
        {
            *this = *this - operand;
        }

        // For convenience. Returns a reference to the value of the specified vector and dimension.
        float& operator()(unsigned int vec, unsigned int dimension)
        {
            return Base::data[vec][dimension];
        }

        // Compare two matrices to see if they're the same
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            bool
        >::type
        operator==(const MatType& operand)
        {
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                for (unsigned int j = 0; j < TotalDimensions; j++) {
                    if (Base::data[i][j] != operand.data[i][j]) {
                        return false;
                    }
                }
            }
            return true;
        }

        // Compare two matrices to see if they're different
        template<typename MatType>
        typename std::enable_if<
            MatType::TotalDimensions == TotalDimensions && MatType::TotalVectors == TotalVectors,
            bool
        >::type
        operator!=(const MatType& operand)
        {
            return !(*this == operand);
        }

        std::string ToString()
        {
            std::string result = "[";
            for (unsigned int i = 0; i < TotalVectors; i++)
            {
                result += "[";
                result += Utilities::ToString(Base::data[i][0]);
                for (unsigned int j = 1; j < TotalDimensions; j++)
                {
                    result += ", ";
                    result += Utilities::ToString(Base::data[i][j]);
                }
                result += "]";
            }
            result += "]";
            return result;
        }

        void FromString(const std::string& str)
        {
            std::string value;
            unsigned int vec = 0;
            unsigned int dim = 0;
            unsigned int closeCount = 0;
            for (unsigned int i = 2, counti = str.length(); i < counti; i++)
            {
                if (str[i] == ',')
                {
                    Utilities::FromString(Base::data[vec][dim], str);
                    value.clear();
                    dim++;
                }
                else if (str[i] == ']')
                {
                    closeCount++;
                    if (closeCount > 1)
                    {
                        break;
                    }
                    dim = 0;
                    vec++;
                }
                else if (str[i] == '[')
                {
                    closeCount--;
                }
                else
                {
                    value += str[i];
                }
            }
        }

        static Matrix<Dimensions, Vectors> Zeroes()
        {
            return zeroes;
        }

        static Matrix<Dimensions, Vectors> Ones()
        {
            return ones;
        }

    private:
        // TODO: Compile time iteration
        /*template<typename MatType, typename N = Vectors>
        inline void CompiledMult(MatType& matrix)
        {
            // Recurse over each element
            CompiledMultValue<MatType, Dimensions>(matrix)
            if constexpr (N > 1)
            {
                CompiledMult<MatType, N - 1>(matrix);
            }
        }

        template<typename MatType, typename Col, typename Element>
        inline void CompiledMultValue(MatType& matrix)
        {
            // Multiply this matrix row
            matrix.data[][] *= data[Col][Element];
            if constexpr (Element > 1)
            {
                CompiledMultValue<MatType, Element - 1>(matrix);
            }
        }*/

    };

    template<unsigned int Dimensions, unsigned int Vectors>
    Matrix<Dimensions, Vectors> Matrix<Dimensions, Vectors>::zeroes = Matrix<Dimensions, Vectors>((float[Vectors][Dimensions]){0});

    template<unsigned int Dimensions, unsigned int Vectors>
    Matrix<Dimensions, Vectors> Matrix<Dimensions, Vectors>::ones = Matrix<Dimensions, Vectors>((float[Vectors][Dimensions]){1});

    struct Vector3 : public Matrix<3, 1>
    {
        Vector3() = default;
        Vector3(float x, float y, float z);
        Vector3(const Matrix<3, 1>& matrix) : Matrix(matrix) {}
        Vector3(const Matrix<2, 1>& matrix) : Matrix(Matrix<3, 1>({{matrix.x, matrix.y, z}})) {}

        Vector3 Cross(const Vector3& vec);
    };

    /// Represents a 2D vector.
    struct Vector2 : public Matrix<2, 1>
    {
        Vector2() = default;
        Vector2(float x, float y) : Matrix({{x, y}}) {}
        Vector2(const Matrix<2, 1>& matrix) : Matrix(matrix) {}
        Vector2(const Matrix<3, 1>& matrix) : Matrix(Matrix<2, 1>({{matrix.x, matrix.y}})) {}

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
        /// Returns a vector rotated by degrees
        Vector2 Rotation(float degrees);
        /// Returns a vector rotated by radians
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

    struct OSSIUM_EDL Circle
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

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

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

        void Draw(Renderer& renderer);
        void Draw(Renderer& renderer, SDL_Color color);

        void DrawFilled(Renderer& renderer);
        void DrawFilled(Renderer& renderer, SDL_Color color);

    };

    /// Represents a general shape
    struct OSSIUM_EDL Polygon
    {
        /// Renders the edges of the polygon
        void Draw(Renderer& renderer);

        /// Renders the polygon in a solid color
        void DrawFilled(Renderer& renderer);

        void Draw(Renderer& renderer, SDL_Color color);
        void DrawFilled(Renderer& renderer, SDL_Color color);

        /// The vertices of the polygon
        std::vector<Point> vertices;

    };

}

#endif // VECTOR_H
