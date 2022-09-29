#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "bgfx/bgfx.h"

#include "callback.h"

namespace Ossium
{
    
    // Abstract class as the target for a renderer
    class RenderTarget
    {
    public:
        // The passed framebuffer will be destroyed automatically when this instance is destroyed, if valid
        RenderTarget() = default;
        ~RenderTarget();

        // Return a handle to the frame buffer
        bgfx::FrameBufferHandle GetFrameBuffer();

        /// Returns the width of this render target.
        virtual int GetWidth() = 0;
        
        /// Returns the height of this render target.
        virtual int GetHeight() = 0;

        // Called whenever the frame buffer is created
        Callback<RenderTarget&> OnRenderTargetReset;

    protected:
        // Frame buffer creation method
        virtual bgfx::FrameBufferHandle CreateFrameBuffer() = 0;

        // The frame buffer rendered to
        bgfx::FrameBufferHandle frameBuffer = BGFX_INVALID_HANDLE;

    };

}

#endif // RENDERTARGET_H
