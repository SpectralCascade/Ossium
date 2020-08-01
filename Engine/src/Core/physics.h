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
#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>
#include <functional>

#include "coremaths.h"

namespace Ossium
{

    inline namespace Physics
    {

        class OSSIUM_EDL OnRayHit : public b2RayCastCallback
        {
        public:
            typedef std::function<float32(b2Fixture*, const Vector2&, const Vector2&, float32)> RayHitCallback;

            OnRayHit(RayHitCallback callback);

            virtual ~OnRayHit() = default;

            float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

        private:
            RayHitCallback onRayHit;
        };

        class OSSIUM_EDL PhysicsWorld : public b2World
        {
        public:
            PhysicsWorld() : b2World({0.0f, -9.81f}) {};
            PhysicsWorld(Vector2 gravity) : b2World(gravity) {};

            /// Overloads that make it easier to perform raycasting and allow for use of Lambdas.
            void RayCast(const Ray& ray, b2RayCastCallback* callback, float distance = 100.0f);
            void RayCast(const Ray& ray, OnRayHit callback, float distance = 100.0f);
            void RayCast(const Point& origin, const Point& endPoint, OnRayHit callback);

        };

    }

}

#endif // PHYSICS_H
