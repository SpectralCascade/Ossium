#ifndef MATRIX_H
#define MATRIX_H

#include <type_traits>
#include <initializer_list>

namespace Ossium
{
    // Base data type for all matrices
    template<unsigned int Dimensions, unsigned int Vectors, typename Enable = void>
    struct MatrixBase
    {
        // The underlying data, in row-major order (for contiguous memory layout).
        float data[Vectors][Dimensions];
    };

    // 2D vector union allows easy access to individual x and y components of a matrix
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

    // 3D vector union allows easy access to individual x, y and z components of a matrix
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

        inline void operator+=(const MatrixBase<3, 1>& operand) {
            x += operand.x;
            y += operand.y;
            z += operand.z;
        }

        inline void operator-=(const MatrixBase<3, 1>& operand) {
            x -= operand.x;
            y -= operand.y;
            z -= operand.z;
        }

    };

    // 4D vector union allows easy access to individual x, y, z and w components of a matrix
    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 4 && Vectors == 1>>
    {
        union {
            struct {
                float x;
                float y;
                float z;
                float w;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];
        };

    };

    // 2x2 matrix union allows easy access to individual a, b, c and d components of a 2x2 matrix
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

    // 3x3 matrix union allows easy access to individual components of a 3x3 matrix
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
    template<unsigned int Dimensions, unsigned int Vectors = Dimensions>
    struct Matrix : public MatrixBase<Dimensions, Vectors>
    {
    private:
        // Base type shorthand
        typedef MatrixBase<Dimensions, Vectors> Base;

    public:
        // Total number of vectors
        inline const static unsigned int TotalVectors = Vectors;

        // Total number of dimensions
        inline const static unsigned int TotalDimensions = Dimensions;

        // Nothing special constructor
        Matrix() = default;

        // Create a matrix with a 2D array
        Matrix(float init[Vectors][Dimensions])
        {
            for (unsigned int i = 0; i < Dimensions; i++)
            {
                for (unsigned int j = 0; j < Vectors; j++)
                {
                    Base::data[i][j] = 0;
                }
            }
        }

        Matrix(const Base& init) : Base(init) {}

        // Create a matrix with an inline 2D array
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
            Matrix<Dimensions, MatType::TotalVectors> result;
            for (unsigned int i = 0; i < MatType::TotalVectors; i++) {
                for (unsigned int j = 0; j < Dimensions; j++) {
                    result.data[i][j] = 0;
                }
            }

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

        // Generate a string representation of this matrix
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

        // Initialise this matrix from a string representation
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
                    Utilities::FromString(Base::data[vec][dim], value);
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

    };

    // Multiply a matrix by a scalar
    template<unsigned int Dimensions, unsigned int Vectors>
    Matrix<Dimensions, Vectors> operator*(float scalar, Matrix<Dimensions, Vectors> mat) {
        return mat * scalar;
    }

    // 4x4 matrix union allows easy access to individual components of a 4x4 matrix
    template<unsigned int Dimensions, unsigned int Vectors>
    struct MatrixBase<Dimensions, Vectors, std::enable_if_t<Dimensions == 4 && Vectors == 4>>
    {
        // Helper struct for ease of use when dealing with 4x4 transform matrices
        struct Scale4x3 {
            Scale4x3() = default;
            Scale4x3(Matrix<3, 1> vec) : x(vec.x), y(vec.y), z(vec.z) {}
            Scale4x3(Matrix<2, 1> vec) : x(vec.x), y(vec.y), z(1) {}

            public: float x;
            private: float _a[4];
            public: float y;
            private: float _b[4];
            public: float z;
            private: float _c;

        public:
            inline void operator+=(const Matrix<3, 1>& operand) {
                x += operand.x;
                y += operand.y;
                z += operand.z;
            }

            inline void operator-=(const Matrix<3, 1>& operand) {
                x -= operand.x;
                y -= operand.y;
                z -= operand.z;
            }
        };

        union {
            struct {
                union {
                    MatrixBase<4, 3> rotation;
                    Scale4x3 scale;
                };
                MatrixBase<3, 1> position;
                float w;
            };

            // The underlying data, in row-major order (for contiguous memory layout).
            float data[Vectors][Dimensions];
        };

    };

}

#endif // MATRIX_H
