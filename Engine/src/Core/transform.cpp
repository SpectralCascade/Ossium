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

    void Transform::RefreshData()
    {
        if (dirty)
        {
            Entity* parent = entity->GetParent();
            if (parent != nullptr)
            {
                world.position = (Matrix<3,1>)local.position + parent->AddComponentOnce<Transform>()->GetWorldPosition();
                Vector3 parentScale = parent->AddComponentOnce<Transform>()->GetWorldScale();
                world.scale = Vector3(local.scale.x * parentScale.x, local.scale.y * parentScale.y, local.scale.z * parentScale.z);
            }
            else
            {
                world.position = (Vector3)local.position;
                world.scale = local.scale;
            }
            dirty = false;
        }
    }

    void Transform::SetRelativeToParent(bool setRelative)
    {
        if (relative != setRelative)
        {
            relative = setRelative;
            SetDirty();
        }
    }

    void Transform::SetDirty()
    {
        if (!dirty)
        {
            dirty = true;
            vector<Transform*> children = entity->GetComponentsInChildren<Transform>();
            for (auto t : children)
            {
                t->SetDirty();
            }
        }
    }

    Vector3 Transform::GetLocalPosition()
    {
        return (Vector3)local.position;
    }

    Vector3 Transform::GetLocalScale()
    {
        return local.scale;
    }

    void Transform::SetLocalPosition(Vector3 p)
    {
        local.position = p;
        SetDirty();
    }

    void Transform::SetLocalScale(Vector3 s)
    {
        local.scale = s;
        SetDirty();
    }

    void Transform::SetLocal(Vector3 p, Vector3 s)
    {
        local.position = p;
        local.scale = s;
        SetDirty();
    }

    Vector3 Transform::GetWorldPosition()
    {
        if (!relative)
        {
            return local.position;
        }
        RefreshData();
        return world.position;
    }

    Vector3 Transform::GetWorldScale()
    {
        if (!relative)
        {
            return local.scale;
        }
        RefreshData();
        return world.scale;
    }

    void Transform::SetWorldPosition(Vector3 p)
    {
        local.position += p - GetWorldPosition();
        SetDirty();
    }

    void Transform::SetWorldScale(Vector3 s)
    {
        local.scale += s - GetWorldScale();
        SetDirty();
    }

    void Transform::SetWorld(Vector3 p, Vector3 s)
    {
        local.position += p - GetWorldPosition();
        local.scale += s - GetWorldScale();
        SetDirty();
    }

    Matrix<4, 4>& Transform::GetLocalMatrix()
    {
        return local;
    }

    Matrix<4, 4>& Transform::GetWorldMatrix()
    {
        return world;
    }

}
