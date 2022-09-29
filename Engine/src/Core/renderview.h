#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <SDL.h>
#include <string>
#include <stack>
#include <vector>
#include "bgfx/bgfx.h"

#include "services.h"
#include "helpermacros.h"

namespace Ossium
{

    class RenderTarget;
    class RenderViewPool;
    class Renderer;

    // A view in which multiple objects can be rendered.
    class RenderView
    {
    private:
        friend class RenderViewPool;

        // Create a RenderView instance. You must provide a unique id.
        RenderView(
            RenderViewPool* pool,
            bgfx::ViewId id,
            SDL_Rect viewport,
            RenderTarget* target,
            std::string name = "RenderView"
        );

        NOCOPY(RenderView);

    public:
        // Get the unique id for this view.
        bgfx::ViewId GetID();

        // Set the render target.
        void SetRenderTarget(RenderTarget* target);

        // Return the render target.
        RenderTarget* GetRenderTarget();

        // Set the render viewport rect.
        void SetViewport(SDL_Rect viewport);

        // Return the render viewport rect.
        SDL_Rect GetViewport();

        // Set the name of this view for graphics debugging purposes.
        void SetDebugName(std::string name);

        // Get the name of this view used for graphics debugging purposes.
        std::string GetDebugName();

    private:
        // The pool which was used to create this render view.
        RenderViewPool* pool;

        // The render target for this view.
        RenderTarget* target;

        // View identifier.
        bgfx::ViewId id;

        // Defines the bounding area of this view.
        SDL_Rect viewport;

        // Name of this view (for debugging purposes only).
        std::string name;

    };

    // Used to create RenderView instances
    class RenderViewPool
    {
    private:
        friend class Renderer;

        // Create a render view in first-come-first-serve order
        RenderView* Create(
            SDL_Rect viewport,
            RenderTarget* target,
            std::string name = "RenderView"
        );

        // Free a RenderView instance by id.
        void Free(bgfx::ViewId id);

        // Free a RenderView instance.
        void Free(RenderView* view);

        // Get a render view by id
        RenderView* Get(bgfx::ViewId id);
        
        // Array of all render view instances
        std::vector<RenderView*> views;

    };

}

#endif // RENDERVIEW_H
