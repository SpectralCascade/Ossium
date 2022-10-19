#include "windowtarget.h"

namespace Ossium
{
    
    bool WindowTarget::Init(uint32_t width, uint32_t height, uint32_t resetFlags, void* nwh)
    {
        bool success = true;
        useBackBuffer = nwh == nullptr;
        if (!useBackBuffer)
        {
            // Setup render target
            frameBuffer = CreateFrameBuffer();
            success = bgfx::isValid(frameBuffer);
            if (!success)
            {
                Log.Error("Failed to create frame buffer for window target.");
            }
        }
        else
        {
            Log.Info("Initialising bgfx...");
            bgfx::PlatformData config;
            config.ndt = nullptr;
            config.nwh = nwh;

            bgfx::Init init;

            // TODO select renderer based on platform
            init.type = bgfx::RendererType::Count;

            // Set rendering resolution
            init.platformData = config;
            // DON'T use virtual getters here as Init() is called in a constructor
            init.resolution.width = width;
            init.resolution.height = height;
            init.resolution.reset = resetFlags;

            success = bgfx::init(init);
            if (!success)
            {
                if (init.type != bgfx::RendererType::Count)
                {
                    Log.Warning(
                        "Could not initialise bgfx with backend: {0}, retrying with default...",
                        bgfx::getRendererName(init.type)
                    );
                    init.type = bgfx::RendererType::Count;
                    success = bgfx::init(init);
                }
                if (!success)
                {
                    Log.Error("Failed to initialise bgfx.");
                }
            }
        }
        return success;
    }

    void WindowTarget::OnWindowDestroyed()
    {
        if (bgfx::isValid(frameBuffer))
        {
            bgfx::destroy(frameBuffer);
            frameBuffer.idx = bgfx::kInvalidHandle;

            // Flush destruction of swap chain (framebuffers for windows are double buffered)
            // Based on https://github.com/bkaradzic/bgfx/blob/master/examples/22-windows/windows.cpp
            bgfx::frame();
            bgfx::frame();
        }
        else if (useBackBuffer)
        {
            bgfx::shutdown();
        }
    }

    void WindowTarget::OnWindowSizeChanged()
    {
        // Must recreate the frame buffer
        // TODO check if necessary for OpenGL, doesn't seem to be for SDL2 when using OpenGL backend
        if (bgfx::isValid(frameBuffer))
        {
            bgfx::destroy(frameBuffer);
            frameBuffer.idx = bgfx::kInvalidHandle;
        }
        frameBuffer = CreateFrameBuffer();
        OnRenderTargetReset(*this);
    }

}

