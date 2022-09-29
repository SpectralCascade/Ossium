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
        virtual bool IsRenderEnabled() { return true; }

        // Return the render debugging name for this input
        virtual std::string GetRenderDebugName() = 0;

    private:
        // Render the input to a target
        virtual void Render(RenderTarget* target) = 0;

        // View to render with
        RenderView* view = nullptr;

    };

}


#endif // RENDERINPUT_H
