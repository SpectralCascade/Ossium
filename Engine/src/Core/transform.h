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
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "coremaths.h"
#include "ecs.h"

namespace Ossium
{

    class Transform;

    struct TransformSchema : public Schema<TransformSchema, 2>
    {
    public:
        DECLARE_BASE_SCHEMA(TransformSchema, 2);

    protected:
        M(Matrix<4>, m) = {0};

        /// Does this transform move relative to the parent?
        M(bool, relative) = true;

    };

    /// TODO: world transform methods
    class OSSIUM_EDL Transform : public BaseComponent, public TransformSchema
    {
    private:
        /// Cached world transform data. Only used if in 'relative' mode.
        /// TODO: Use a single matrix!
        Vector3 worldPosition = Vector3::Zeroes;
        Vector3 worldScale = Vector3::Ones;

        /// Has this transform been modified?
        bool dirty = true;

        /// Refresh the world position data
        void RefreshData();

    public:
        DECLARE_COMPONENT(BaseComponent, Transform);
        CONSTRUCT_SCHEMA(BaseComponent, TransformSchema);

        /// Set relative to the parent transform.
        void SetRelativeToParent(bool setRelative);

        void SetDirty();

        Vector3 GetLocalPosition();
        Vector3 GetLocalScale();

        void SetLocalPosition(Vector3 p);
        void SetLocalScale(Vector3 s);
        void SetLocal(Vector3 p, Vector3 s);

        Vector3 GetWorldPosition();
        Vector3 GetWorldScale();

        void SetWorldPosition(Vector3 p);
        void SetWorldScale(Vector3 s);
        void SetWorld(Vector3 p, Vector3 s);

    };

}

#endif // TRANSFORM_H
