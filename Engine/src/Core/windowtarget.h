#ifndef WINDOWTARGET_H
#define WINDOWTARGET_H

#include "rendertarget.h"

namespace Ossium
{
    
    // RenderTarget for a window
    class WindowTarget : RenderTarget
    {
    protected:
        WindowTarget() = default;

        // Pass on the frame buffer creation method.
        virtual bgfx::FrameBufferHandle CreateFrameBuffer() = 0;

        /// Pass on the window width getter.
        virtual int GetWidth() = 0;

        /// Pass on the window height getter.
        virtual int GetHeight() = 0;

        // Initialise this target to either create a frame buffer or use the backbuffer
        bool Init(uint32_t width, uint32_t height, uint32_t resetFlags, void* nwh);

        /// Called when the associated window is destroyed.
        void OnWindowDestroyed();

        // Called when the associated window size is changed.
        void OnWindowSizeChanged();

        // Should this window use the backbuffer rather than a dedicated frame buffer?
        bool useBackBuffer = false;
    };

}

#endif // WINDOWTARGET_H
