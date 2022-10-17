#include "canvas.h"

namespace Ossium
{

    REGISTER_COMPONENT(Canvas);

    void Canvas::OnCreate()
    {
        Renderer* renderer = GetEntity()->GetService<Renderer>();
        if (renderer != nullptr)
        {
            renderer->AddInput(this);
        }
    }

    void Canvas::OnDestroy()
    {
        Renderer* renderer = GetEntity()->GetService<Renderer>();
        if (renderer != nullptr)
        {
            renderer->RemoveInput(this);
        }
    }

    std::string Canvas::GetRenderDebugName()
    {
        return GetEntity()->name;
    }

    int Canvas::GetViewMode()
    {
        // TODO: Consider using Z-buffer instead for 2D GUI
        return bgfx::ViewMode::Sequential;
    }

    void Canvas::Render()
    {
        Entity* root = GetEntity();
        root->GetScene()->WalkEntities([&] (Entity* child) {
            
            bool result = child->IsActive() && !child->HasComponent<Canvas>();
            if (result)
            {
                auto graphics = child->GetComponents<GraphicComponent>();
                for (GraphicComponent* graphic : graphics)
                {
                    if (graphic->IsEnabled())
                    {
                        graphic->Render(this);
                    }
                }
            }
            return result;
        }, true, root);
    }

}
