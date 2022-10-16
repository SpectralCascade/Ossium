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
#include "transform.h"

using namespace std;

namespace Ossium
{

    REGISTER_COMPONENT(Transform);

    void Transform::SetRelativeToParent(bool setRelative)
    {
        if (relative != setRelative)
        {
            relative = setRelative;
            dirty = true;
        }
    }

    void Transform::SetDirty()
    {
        dirty = true;
    }

    bool Transform::IsDirty()
    {
        Entity* parent = entity->GetParent();
        dirty = dirty || (parent != nullptr && parent->AddComponentOnce<Transform>()->IsDirty());
        return dirty;
    }

    Vector3 Transform::GetLocalPosition()
    {
        return position;
    }

    Vector3 Transform::GetLocalScale()
    {
        return scale;
    }

    void Transform::SetLocalPosition(Vector3 p)
    {
        position = p;
        dirty = true;
    }

    void Transform::SetLocalScale(Vector3 s)
    {
        scale = s;
        dirty = true;
    }

    Vector3 Transform::GetWorldPosition()
    {
        if (!relative)
        {
            return position;
        }
        auto mat = GetMatrix();
        return Vector3(mat(3, 0), mat(3, 1), mat(3, 2));
    }

    void Transform::SetWorldPosition(Vector3 p)
    {
        position += p - GetWorldPosition();
        dirty = true;
    }

    Matrix<4, 4> Transform::GetMatrix()
    {
        Entity* parent = entity->GetParent();
        if (parent != nullptr)
        {
            Transform* parentT = parent->GetComponent<Transform>();
            if (parentT != nullptr && IsDirty())
            {
                // Build cache from local transform data and parent transform
                cache = {
                    {scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1}
                };
                Matrix<4, 4> translation = {
                    {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {position.x, position.y, position.z, 1}
                };
                cache = (relative ? parentT->GetMatrix() : (Matrix<4, 4>){
                    {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}
                }) * (translation * (rotation * cache));

                dirty = false;
            }
        }
        return cache;
    }

}
