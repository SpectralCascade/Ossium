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

    class RenderViewPool;

    // A view which graphics can be rendered to.
    class RenderView
    {
    private:
        friend RenderViewPool;

        // Create a RenderView instance. You must provide a unique id.
        RenderView(
            bgfx::ViewId id,
            SDL_Rect rect,
            bgfx::FrameBufferHandle target = BGFX_INVALID_HANDLE,
            std::string name = "RenderView"
        );

        NOCOPY(RenderView);

    public:
        // Set the target frame buffer that should be rendered to by this view.
        // If the given target is invalid, the main window backbuffer is used instead by default.
        void SetRenderTarget(bgfx::FrameBufferHandle target);

        // Returns the target frame buffer handle
        bgfx::FrameBufferHandle GetRenderTarget();

        // Set the render view bounding rect
        void SetRenderRect(SDL_Rect rect);

        // Return the render view bounding rect
        SDL_Rect GetRenderRect();

        // Get this view's identifier
        bgfx::ViewId GetID();

        // Set the name of this view for graphics debugging purposes
        void SetDebugName(std::string name);

        // Get the name of this view used for graphics debugging purposes
        std::string GetDebugName();

    private:
        // View identifier
        bgfx::ViewId id;

        // The target frame buffer
        bgfx::FrameBufferHandle target;

        // Defines the bounding area of this view
        SDL_Rect rect;

        // Name of this view (for debugging purposes only)
        std::string name;

    };

    // Used to create RenderView instances
    class RenderViewPool
    {
    public:
        // Initialise the render view pool
        RenderViewPool(size_t prealloc = 8);

        // Create a render view
        bgfx::ViewId Create(
            SDL_Rect rect,
            bgfx::FrameBufferHandle target = BGFX_INVALID_HANDLE,
            std::string name = "RenderView"
        );

        // Free a render view
        void Free(bgfx::ViewId id);

        // Get a render view by id
        RenderView& Get(bgfx::ViewId id);

    private:
        // Head of the pool
        size_t head = 0;

        // Array of all render view instances
        std::vector<RenderView> views;

        // Stack of freed ids
        std::stack<size_t> ids;

    }

}

#endif // RENDERVIEW_H
