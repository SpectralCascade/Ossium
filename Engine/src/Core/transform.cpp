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
                worldPosition = (Matrix<3,1>)m.position + parent->AddComponentOnce<Transform>()->GetWorldPosition();
                Vector3 parentScale = parent->AddComponentOnce<Transform>()->GetWorldScale();
                worldScale = Vector3(m.scale.x * parentScale.x, m.scale.y * parentScale.y, m.scale.z * parentScale.z);
            }
            else
            {
                worldPosition = (Vector3)m.position;
                worldScale = m.scale;
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
        return (Vector3)m.position;
    }

    Vector3 Transform::GetLocalScale()
    {
        return m.scale;
    }

    void Transform::SetLocalPosition(Vector3 p)
    {
        m.position = p;
        SetDirty();
    }

    void Transform::SetLocalScale(Vector3 s)
    {
        m.scale = s;
        SetDirty();
    }

    void Transform::SetLocal(Vector3 p, Vector3 s)
    {
        m.position = p;
        m.scale = s;
        SetDirty();
    }

    Vector3 Transform::GetWorldPosition()
    {
        if (!relative)
        {
            return m.position;
        }
        RefreshData();
        return worldPosition;
    }

    Vector3 Transform::GetWorldScale()
    {
        if (!relative)
        {
            return m.scale;
        }
        RefreshData();
        return worldScale;
    }

    void Transform::SetWorldPosition(Vector3 p)
    {
        m.position += p - GetWorldPosition();
        SetDirty();
    }

    void Transform::SetWorldScale(Vector3 s)
    {
        m.scale += s - GetWorldScale();
        SetDirty();
    }

    void Transform::SetWorld(Vector3 p, Vector3 s)
    {
        m.position += p - GetWorldPosition();
        m.scale += s - GetWorldScale();
        SetDirty();
    }

}
