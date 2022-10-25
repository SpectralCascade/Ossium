#include "camera.h"

namespace Ossium
{
    
    REGISTER_COMPONENT(Camera);

    void Camera::OnCreate()
    {
        GetRenderer()->AddInput(this);
    }

    void Camera::OnDestroy()
    {
        GetRenderer()->RemoveInput(this);
    }

    std::string Camera::GetRenderDebugName()
    {
        return "Camera." + GetEntity()->name;
    }
    
    bgfx::ViewMode::Enum Camera::GetViewMode()
    {
        return bgfx::ViewMode::Default;
    }

    Matrix<4, 4> Camera::GetViewMatrix()
    {
        auto t = GetEntity()->AddComponentOnce<Transform>();
        // TODO get transform orientation for up direction and at position
        Vector3 pos = t->GetWorldPosition();
        return Matrix<4, 4>::LookAt(pos, Vector3(0, 1, 0), pos + Vector3(0, 0, 1));
    }

    Matrix<4, 4> Camera::GetProjMatrix()
    {
        return Matrix<4, 4>::Perspective(fov, GetRenderer()->GetAspectRatio(), near, far);
    }

    void Camera::Render()
    {
        // Cameras render all RenderComponent instances.
        Renderer* renderer = GetRenderer();
        Matrix<4, 4> view = GetViewMatrix();
        Matrix<4, 4> proj = GetProjMatrix();
        GetEntity()->GetScene()->WalkEntities([&] (Entity* target) {
            if (!target->IsActive())
            {
                return false;
            }
            auto renderables = target->GetComponents<RenderComponent>();
            for (auto toRender : renderables)
            {
                if (toRender->IsEnabled())
                {
                    toRender->Render(this, view, proj);
                }
            }
            return true;
        });
    }

}
