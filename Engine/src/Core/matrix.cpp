#include "matrix.h"
#include "coremaths.h"

namespace Ossium
{
    Matrix<4, 4> Matrix<4, 4>::LookAt(Vector3 from, Vector3 up, Vector3 at)
    {
        Vector3 direction = Vector3(at - from).Normalised();
        up = up.Normalised();
        Vector3 crossed = direction.Cross(up);
        Vector3 invcrossed = crossed.Normalised().Cross(direction);
        return Matrix<4, 4>({
            {crossed.x, crossed.y, crossed.z, 0},
            {invcrossed.x, invcrossed.y, invcrossed.z, 0},
            {-direction.x, -direction.y, -direction.z, 0},
            {0, 0, 0, 1}
        });
    }
}
