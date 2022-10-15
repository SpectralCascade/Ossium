#ifndef RENDERINPUT_H
#define RENDERINPUT_H

#include <string>

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

    protected:
        // Return the associated render view
        RenderView* GetRenderView();

        // Return the mode that determines draw call order, e.g. z-buffer depth or sequential rendering
        virtual int GetViewMode() = 0;

    private:
        // Render this RenderInput to a target
        virtual void Render(Renderer* renderer) = 0;

        // View to render
        RenderView* renderView = nullptr;

    };

}

#endif // RENDERINPUT_H
