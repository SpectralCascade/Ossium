#include "rendertarget.h"

namespace Ossium
{
    
    RenderTarget::~RenderTarget()
    {
        if (bgfx::isValid(frameBuffer))
        {
            bgfx::destroy(frameBuffer);
        }
    }

    bgfx::FrameBufferHandle RenderTarget::GetFrameBuffer()
    {
        return frameBuffer;
    }

}
