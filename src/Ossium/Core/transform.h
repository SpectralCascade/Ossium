/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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

    struct Transform2Schema : public Schema<Transform2Schema, 2>
    {
    public:
        DECLARE_BASE_SCHEMA(Transform2Schema, 2);

        /// Should this transform be tied to the parent transform?
        M(bool, tieToParent) = false;

    protected:
        /// Local/relative transform to parent.
        M(Transform, trans);

    };

    class OSSIUM_EDL Transform2 : public BaseComponent, public GlobalServices, public Transform2Schema
    {
    public:
        DECLARE_COMPONENT(Transform2);
        CONSTRUCT_SCHEMA(BaseComponent, Transform2Schema);

        /// Ref to the local transform position.
        Point& position();

        /// Ref to the local transform orientation.
        Rotation& rotation();

        /// Ref to the local transform itself.
        Transform& transform();

    };

}

#endif // TRANSFORM_H
