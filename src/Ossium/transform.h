#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"

namespace Ossium
{
    /// 2x2 matrix with additional scaling component indicating position and orientation
    /// Each vector represents a column in the matrix, for ease of use
    struct Transform
    {
        Vector position;
        Vector direction;
        Vector scale;
    };

    /// Transform a vector by multiplying with a transform matrix
    Vector operator*(const Vector& vec, const Transform& matrixTransform);
    Vector operator*(const Transform& matrixTransform, const Vector& vec);

}

#endif // TRANSFORM_H
