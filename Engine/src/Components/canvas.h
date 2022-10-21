#ifndef CANVAS_H
#define CANVAS_H

#include "../Core/component.h"
#include "../Core/renderinput.h"
#include "../Core/rendertarget.h"

namespace Ossium
{
    
    struct CanvasSchema : public Schema<CanvasSchema, 20>
    {
        DECLARE_BASE_SCHEMA(CanvasSchema, 20);
        
    };
    
    // A canvas is a RenderInput that renders child graphics sequentially (like painting layers on a canvas)
    // This is effectively a 2D camera that moves the origin to the upper left,
    // rendering sequentially instead of sorting by depth as a 3D camera would.
    class Canvas : public RenderInput, public Component, public CanvasSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, CanvasSchema);
        DECLARE_COMPONENT(Component, Canvas);

        // Add RenderInput to main renderer
        void OnCreate();

        // Remove RenderInput from main renderer
        void OnDestroy();

        // Returns a name for graphics debugging purposes
        std::string GetRenderDebugName();
        
        // Always returns sequential draw call order mode
        bgfx::ViewMode::Enum GetViewMode();

        // Render all child graphics
        void Render();
        
    };
    
}

#endif // CANVAS_H
