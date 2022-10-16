/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include <queue>
#include <algorithm>
#include <unordered_set>

#include "renderer.h"
#include "window.h"
#include "coremaths.h"
#include "colors.h"
#include "logging.h"

using namespace std;

namespace Ossium
{

    Renderer::Renderer(RenderTarget* target, RenderViewPool* renderViewPool)
    {
#ifdef OSSIUM_DEBUG
        DEBUG_ASSERT(target != NULL, "Render target must not be null.");
        DEBUG_ASSERT(renderViewPool != NULL, "Render view pool must not be null.");
#endif
        this->target = target;
        this->renderViewPool = renderViewPool;

        aspect_width = 0;
        aspect_height = 0;
        fixed_aspect = false;

        viewportRect.x = 0;
        viewportRect.y = 0;
        viewportRect.w = target->GetWidth();
        viewportRect.h = target->GetHeight();
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::RenderPresent()
    {
        // Always clear the render target view
        // TODO this may not be necessary
        for (unsigned int i = 0, counti = inputs.size(); i < counti; i++)
        {
            bgfx::setViewClear(
                inputs[i]->renderView->GetID(),
                BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, // Clear flags
                ColorToUint32(bufferColour, SDL_PIXELFORMAT_ABGR8888) // TODO check this is correct
            );
        }
        
        // Render pipeline inputs
        for (unsigned int i = 0, counti = inputs.size(); i < counti; i++)
        {
            if (inputs[i]->IsRenderEnabled())
            {
                inputs[i]->Render();
            }
        }

        #ifdef OSSIUM_DEBUG
        numRenderedPrevious = numRendered;
        numRendered = 0;
        #endif // DEBUG
        SetDrawColor(bufferColour);

        // Actually render everything
        bgfx::frame();
    }

    void Renderer::SetDrawColor(SDL_Color color)
    {
        drawColour = color;
    }

    void Renderer::SetDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
    {
        drawColour = { .r=red, .g=green, .b=blue, .a=alpha };
    }

    SDL_Color Renderer::GetDrawColor()
    {
        return drawColour;
    }

    Uint32 Renderer::GetDrawColorUint32()
    {
        return ColorToUint32(drawColour, SDL_PIXELFORMAT_ABGR8888);
    }

    void Renderer::SetState(Uint64 state)
    {
        this->state = state;
    }

    Uint64 Renderer::GetState()
    {
        return state;
    }

    void Renderer::UpdateStateAndColor()
    {
        // TODO check pixel format is correct
        bgfx::setState(state, GetDrawColorUint32());
    }

    int Renderer::GetWidth()
    {
        return viewportRect.w;
    }
    /// Returns the viewport height of this renderer
    int Renderer::GetHeight()
    {
        return viewportRect.h;
    }

    int Renderer::GetAspectWidth()
    {
        return aspect_width;
    }

    int Renderer::GetAspectHeight()
    {
        return aspect_height;
    }

    SDL_Rect Renderer::GetViewportRect()
    {
        return viewportRect;
    }

    void Renderer::SetViewportRect(SDL_Rect rect)
    {
        aspect_width = (int)rect.w;
        aspect_height = (int)rect.h;
        viewportRect = rect;
        bgfx::setViewRect(0, viewportRect.x, viewportRect.y, viewportRect.w, viewportRect.h);
    }

    void Renderer::TargetToViewportPoint(int& x, int& y)
    {
        y -= viewportRect.y;
        x -= viewportRect.x;
    }

    void Renderer::ResetViewport()
    {
        // TODO
        /*if (target == nullptr)
        {
            Log.Warning("Failed to reset renderer viewport! Associated target is NULL.");
            // Early out
            return;
        }
        SDL_Rect viewRect;
        float percent_width = 1.0f;
        float percent_height = 1.0f;
        int width = target->GetWidth();
        int height = target->GetHeight();
        Rect displayRect = renderWindow->GetDisplayBounds();

        if (aspect_width > 0 && aspect_height > 0)
        {
            if (renderWindow->IsFullscreen())
            {
                percent_width = (float)displayRect.w / (float)aspect_width;
                percent_height = (float)displayRect.h / (float)aspect_height;
            }
            else
            {
                percent_width = (float)width / (float)aspect_width;
                percent_height = (float)height / (float)aspect_height;
            }
            /// Get the smallest percent and use that to scale dimensions
            float smallest_percent;
            if (percent_width < percent_height)
            {
                smallest_percent = percent_width;
            }
            else
            {
                smallest_percent = percent_height;
            }
            if (fixed_aspect)
            {
                smallest_percent = Clamp(smallest_percent, 0.0f, 1.0f);
            }
            viewRect.h = (int)(smallest_percent * (!renderWindow->IsFullscreen() ? (float)aspect_height : (float)displayRect.h));
            viewRect.w = (int)(smallest_percent * (!renderWindow->IsFullscreen() ? (float)aspect_width : (float)displayRect.w));

            /// Calculate viewport anchor position
            int deltaw = (width - viewRect.w);
            int deltah = (height - viewRect.h);
            if (deltaw > 0)
            {
                viewRect.x = deltaw / 2;
            }
            else
            {
                viewRect.x = 0;
            }
            if (deltah > 0)
            {
                viewRect.y = deltah / 2;
            }
            else
            {
                viewRect.y = 0;
            }
        }
        else
        {
            /// No aspect ratio is set
            viewRect.x = 0;
            viewRect.y = 0;
            viewRect.w = target->GetWidth();
            viewRect.h = target->GetHeight();
        }

        bgfx::setViewRect(mainView, viewRect.x, viewRect.y, viewRect.w, viewRect.h);

        viewportRect = viewRect;
        */
    }

    void Renderer::SetAspectRatio(int aspect_w, int aspect_h, bool fixed, bool resetViewport)
    {
        fixed_aspect = fixed;
        if (aspect_w < 1)
        {
            aspect_w = 1;
        }
        if (aspect_h < 1)
        {
            aspect_h = 1;
        }
        aspect_width = aspect_w;
        aspect_height = aspect_h;
        if (resetViewport)
        {
            ResetViewport();
        }
    }

    void Renderer::AddInput(RenderInput* input)
    {
        input->renderView = renderViewPool->Create(viewportRect, target, input->GetRenderDebugName());
        input->renderer = this;
        inputs.push_back(input);
    }

    void Renderer::RemoveInput(RenderInput* input)
    {
        unsigned int i = 0;
        unsigned int counti = inputs.size();
        for (; i < counti; i++)
        {
            if (input == inputs[i])
            {
                // Shift down the render inputs chain
                for (; i < counti - 1; i++)
                {
                    inputs[i] = inputs[i + 1];
                }
                inputs.pop_back();
                renderViewPool->Free(input->renderView);
                input->renderView = nullptr;
                input->renderer = nullptr;
                return;
            }
        }
        Log.Error(
            "Failed to locate RenderInput instance \"{0}\" in Renderer instance.",
            input->GetRenderDebugName()
        );
    }

    // GENERAL

    #ifdef OSSIUM_DEBUG
    int Renderer::GetNumRendered()
    {
        return numRenderedPrevious;
    }
    #endif // DEBUG

    SDL_Color Renderer::GetBackgroundColor()
    {
        return bufferColour;
    }

    void Renderer::SetBackgroundColor(SDL_Color color)
    {
        bufferColour = color;
    }

}
