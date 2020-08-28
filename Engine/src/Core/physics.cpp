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
#include "physics.h"

namespace Ossium
{

    OnRayHit::OnRayHit(RayHitCallback callback)
    {
        onRayHit = callback;
    }

    float32 OnRayHit::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
    {
        return onRayHit(fixture, Vector2(point), Vector2(normal), fraction);
    }

    void PhysicsWorld::RayCast(const Ray& ray, b2RayCastCallback* callback, float distance)
    {
        Vector2 endPoint = ray.u;
        endPoint = ray.p + (endPoint.Normalized() * distance);
        b2World::RayCast(callback, ray.p, endPoint);
    }

    void PhysicsWorld::RayCast(const Ray& ray, OnRayHit* callback, float distance)
    {
        RayCast(ray, callback, distance);
    }

    void PhysicsWorld::RayCast(const Point& origin, const Point& endPoint, OnRayHit* callback)
    {
        b2World::RayCast(callback, origin, endPoint);
    }

    void PhysicsWorld::PostUpdate()
    {
        if (!frozen)
        {
            Step(timeStep, velocityIterations, positionIterations);
        }
    }

    void PhysicsWorld::SetFrozen(bool freeze)
    {
        if (!freeze && frozen)
        {
            // Clear all forces
            ClearForces();
        }
        frozen = freeze;
    }

    bool PhysicsWorld::IsFrozen()
    {
        return frozen;
    }

}
