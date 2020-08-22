#include "collider.h"

namespace Ossium
{

    REGISTER_COMPONENT(PhysicsBody);

    // Use OnLoadFinish()
    void PhysicsBody::OnLoadFinish()
    {
        ParentType::OnLoadFinish();

#ifndef OSSIUM_EDITOR
        Physics::PhysicsWorld* world = entity->GetService<Physics::PhysicsWorld>();
        DEBUG_ASSERT(world != nullptr, "Physics world cannot be NULL");
        Collider* collider = entity->GetComponent<Collider>();
        if (body == nullptr && collider != nullptr)
        {
            // Define the body
            b2BodyDef bodyDef;
            bodyDef.position.Set(PTM(GetTransform()->GetWorldPosition().x), PTM(GetTransform()->GetWorldPosition().y));
            bodyDef.angle = GetTransform()->GetWorldRotation().GetRadians();
            bodyDef.type = bodyType;
            bodyDef.active = IsActiveAndEnabled();
            bodyDef.awake = startAwake;
            bodyDef.allowSleep = allowSleep;
            bodyDef.angularDamping = angularDamping;
            bodyDef.angularVelocity = initialAngularVelocity;
            bodyDef.linearDamping = linearDamping;
            bodyDef.linearVelocity = initialLinearVelocity;
            bodyDef.bullet = bullet;
            bodyDef.fixedRotation = fixedRotation;
            bodyDef.gravityScale = gravityScale;
            // Create the body
            body = world->CreateBody(&bodyDef);
            // Define the fixture
            b2FixtureDef fixDef;
            fixDef.shape = &collider->GetShape();
            fixDef.density = density;
            fixDef.friction = friction;
            fixDef.isSensor = sensor;
            // Create the fixture
            fixture = body->CreateFixture(&fixDef);
        }
        // TODO: update properties if body and fixture are already created.
#endif // OSSIUM_EDITOR
    }

    void PhysicsBody::OnDestroy()
    {
        PhysicsBody::ParentType::OnDestroy();
        if (body != nullptr)
        {
            Physics::PhysicsWorld* world = entity->GetService<Physics::PhysicsWorld>();
            DEBUG_ASSERT(world != nullptr, "Physics world cannot be NULL");
            // Body will cleanup any attached fixtures
            world->DestroyBody(body);
            body = nullptr;
            fixture = nullptr;
        }
    }

    void PhysicsBody::UpdatePhysics()
    {
        if (body != nullptr)
        {
            // Update location and rotation in-game.
            const b2Transform& b2t = body->GetTransform();
            Transform* t = GetTransform();
            t->SetWorldPosition(Vector2(MTP(b2t.p.x), MTP(b2t.p.y)));
            t->SetWorldRotation(Rotation(b2t.q));
        }
    }

    REGISTER_ABSTRACT_COMPONENT(Collider);

}
