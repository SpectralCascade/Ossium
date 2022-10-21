#ifndef RENDERINPUT_H
#define RENDERINPUT_H

#include <string>
#include "bgfx/bgfx.h"

namespace Ossium
{

    class RenderTarget;
    class Renderer;
    class RenderView;
    
    // An abstract class that takes represents a render input such as a camera, GUI canvas,
    // or even a RenderTarget instance (e.g. for doing things like shadow mapping or deferred rendering)
    class RenderInput
    {
    public:
        friend class Renderer;

        // Should this input be rendered?
        virtual bool IsRenderEnabled();

        // Return the render debugging name for this input
        virtual std::string GetRenderDebugName() = 0;

        // Return the associated renderer
        Renderer* GetRenderer();

        // Return the associated ViewID for this input
        uint16_t GetID();

    protected:
        // Return the mode that determines draw call order, e.g. z-buffer depth or sequential rendering
        virtual bgfx::ViewMode::Enum GetViewMode();

        // Return the associated render view
        RenderView* GetRenderView();

    private:
        // Render this RenderInput to a target
        virtual void Render() = 0;

        // View to render
        // There may only be one view per RenderInput instance
        RenderView* renderView = nullptr;

        // The associated renderer accepting this input
        // There may only be one renderer per RenderInput instance
        Renderer* renderer = nullptr;

    };

}

#endif // RENDERINPUT_H
