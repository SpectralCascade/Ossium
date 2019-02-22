#include "vector.h"
#include "transform.h"

namespace Ossium
{

    inline Vector operator*(const Vector& vec, const Transform& matrixTransform)
    {
        return (vec.x * matrixTransform.position) + (vec.y * matrixTransform.direction);
    }

    inline Vector operator*(const Transform& matrixTransform, const Vector& vec)
    {
        return vec * matrixTransform;
    }

}
