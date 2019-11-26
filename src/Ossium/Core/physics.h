#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>
#include <functional>

#include "coremaths.h"

using namespace std;

namespace Ossium
{

    inline namespace Physics
    {

        class OSSIUM_EDL OnRayHit : public b2RayCastCallback
        {
        public:
            typedef function<float32(b2Fixture*, const Vector2&, const Vector2&, float32)> RayHitCallback;

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
