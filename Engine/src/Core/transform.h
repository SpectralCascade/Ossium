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

    struct TransformSchema : public Schema<TransformSchema, 4>
    {
    public:
        DECLARE_BASE_SCHEMA(TransformSchema, 4);

    protected:
        // TODO minor optimisation for full release/non-editor builds: don't initialise these
        M(Vector3, position) = {{0, 0, 0}};
        M(Vector3, scale) = {{1, 1, 1}};
        // TODO use quaternion instead
        M(Matrix<4>, rotation) = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

        /// Does this transform move relative to the parent?
        M(bool, relative) = true;

    };

    /// TODO: world transform methods
    class OSSIUM_EDL Transform : public BaseComponent, public TransformSchema
    {
    private:
        // The only purpose of this matrix is to cache a transformation for whatever hierarchy is in use
        Matrix<4, 4> cache = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

        /// Has this transform or it's parent hierarchy been modified?
        bool dirty = true;

    public:
        DECLARE_COMPONENT(BaseComponent, Transform);
        CONSTRUCT_SCHEMA(BaseComponent, TransformSchema);

        /// Set relative to the parent transform.
        void SetRelativeToParent(bool setRelative);

        // Mark this transform as having changes
        void SetDirty();

        // Check if this transform has changes
        bool IsDirty();

        // Get the local position of this transform
        Vector3 GetLocalPosition();

        // Get the local scale of this transform
        Vector3 GetLocalScale();

        // Set the local position of this transform
        void SetLocalPosition(Vector3 p);

        // Set the local scale of this transform
        void SetLocalScale(Vector3 s);

        // Get the position of this transform relative to parent transforms
        Vector3 GetWorldPosition();

        // Set the position of this transform relative to parent transforms
        void SetWorldPosition(Vector3 p);
        
        // Return the matrix representing this transform in world space
        Matrix<4, 4> GetMatrix();

    };

}

#endif // TRANSFORM_H
