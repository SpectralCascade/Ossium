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
#include "globalservices.h"

namespace Ossium
{

    class Transform;

    struct TransformSchema : public Schema<TransformSchema, 4>
    {
    public:
        DECLARE_BASE_SCHEMA(TransformSchema, 4);

    protected:
        /// Local/relative position to parent.
        M(Point, position) = Point(0, 0);
        /// Local/relative rotation to parent.
        M(Rotation, rotation) = Rotation(0);
        /// Local/relative scale to parent.
        M(Vector2, scale) = Vector2(1, 1);

        /// Does this transform move relative to the parent?
        M(bool, relative) = true;

    };

    /// TODO: world transform methods
    class OSSIUM_EDL Transform : public BaseComponent, public GlobalServices, public TransformSchema
    {
    private:
        /// Cached world transform data. Only used if in 'relative' mode.
        Point worldPosition = Point(0, 0);
        Rotation worldRotation = Rotation(0);
        Vector2 worldScale = Vector2(1, 1);

        /// Does this transform follow the parent transform?
        bool dirty = true;

        void SetDirty();

    public:
        DECLARE_COMPONENT(BaseComponent, Transform);
        CONSTRUCT_SCHEMA(BaseComponent, TransformSchema);

        Point GetLocalPosition();
        Rotation GetLocalRotation();
        Vector2 GetLocalScale();

        void SetLocalPosition(Point p);
        void SetLocalRotation(Rotation r);
        void SetLocalScale(Vector2 s);
        void SetLocal(Point p, Rotation r, Vector2 s);

        Point GetWorldPosition();
        Rotation GetWorldRotation();
        Vector2 GetWorldScale();

        void SetWorldPosition(Point p);
        void SetWorldRotation(Rotation r);
        void SetWorldScale(Vector2 s);
        void SetWorld(Point p, Rotation r, Vector2 s);

    };

}

#endif // TRANSFORM_H
