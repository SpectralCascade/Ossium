#include "physics.h"

namespace Ossium
{

    OnRayHit::OnRayHit(RayHitCallback callback)
    {
        onRayHit = callback;
    }

    float32 OnRayHit::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
    {
        return onRayHit(fixture, EVIL_CAST(point, Point), EVIL_CAST(normal, Vector2), fraction);
    }

    void PhysicsWorld::RayCast(const Ray& ray, b2RayCastCallback* callback, float distance)
    {
        Vector2 endPoint = ray.u;
        endPoint = ray.p + (endPoint.Normalized() * distance);
        b2World::RayCast(callback,
                EvilCast<b2Vec2>(ray.p),
                EvilCast<b2Vec2>(endPoint)
        );
    }

    void PhysicsWorld::RayCast(const Ray& ray, OnRayHit callback, float distance)
    {
        RayCast(ray, &callback, distance);
    }

    void PhysicsWorld::RayCast(const Point& origin, const Point& endPoint, OnRayHit callback)
    {
        b2World::RayCast(&callback,
                EvilCast<b2Vec2>(origin),
                EvilCast<b2Vec2>(endPoint)
        );
    }

}
