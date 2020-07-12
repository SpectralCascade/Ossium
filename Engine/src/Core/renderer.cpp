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

using namespace std;

namespace Ossium
{

    Renderer::Renderer(Window* window, int numLayers, Uint32 flags, int driver)
    {
        #ifdef OSSIUM_DEBUG
        SDL_assert(window != NULL);
        #endif // DEBUG

        aspect_width = 0;
        aspect_height = 0;
        fixed_aspect = false;

        renderer = SDL_CreateRenderer(window->GetWindowSDL(), driver, flags);
        if (renderer == NULL)
        {
            Log.Error("[Renderer] Could not create renderer! SDL_Error: {0}", SDL_GetError());
            int n_drivers = SDL_GetNumRenderDrivers();
            SDL_RendererInfo driver_data;
            string drivers_available;
            for (int i = 0; i < n_drivers; i++)
            {
                SDL_GetRenderDriverInfo(i, &driver_data);
                drivers_available = drivers_available + driver_data.name + ", ";
            }
            Log.Info("Available render drivers are: {0}", drivers_available);
            Log.Info("Falling back to software renderer by default.");
            renderer = SDL_CreateRenderer(window->GetWindowSDL(), driver, SDL_RENDERER_SOFTWARE);
            if (renderer == NULL)
            {
                Log.Error("[Renderer] Fallback software renderer could not be created! SDL_Error: {0}", SDL_GetError());
            }
        }
        if (renderer != NULL)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }
        registeredGraphics = new set<Graphic*>[numLayers];
        queuedGraphics = new queue<Graphic*>[numLayers];

        /// No need to store id as the OnDestroyed callback is automatically freed after being called.
        window->OnDestroyed += [&] (Window& win) { this->OnWindowDestroyed(win); };

        viewportRect.x = 0;
        viewportRect.y = 0;
        viewportRect.w = window->GetWidth();
        viewportRect.h = window->GetHeight();

        numLayersActive = numLayers;
        renderWindow = window;
    }

    Renderer::~Renderer()
    {
        if (renderWindow != nullptr)
        {
            renderWindow = nullptr;
        }

        SDL_RenderClear(renderer);
        SDL_DestroyRenderer(renderer);
        renderer = NULL;

        delete[] registeredGraphics;
        delete[] queuedGraphics;
    }

    void Renderer::OnWindowDestroyed(Window& windowCaller)
    {
        renderWindow = nullptr;
        /// No need to actually unregister the callbacks as the window will destroy it's callback objects.
    }

    int Renderer::Register(Graphic* graphic, int layer)
    {
        int intendedLayer = layer;
        layer = Clamp(layer, 0, numLayersActive);
        if (layer != intendedLayer)
        {
            Log.Warning("[Renderer] Registered graphic on layer [{0}] because the intended layer [{1}] is out of bounds (max layer is [{2}]).", layer, intendedLayer, numLayersActive - 1);
        }
        registeredGraphics[layer].insert(graphic);
        return layer;
    }

    void Renderer::Unregister(Graphic* graphic, int layer)
    {
        if (!(layer >= 0 && layer < numLayersActive))
        {
            Log.Error("[Renderer] Failed to unregister a graphic because the intended layer [{0}] is out of bounds (max layer is {1}).", layer, numLayersActive - 1);
            return;
        }
        registeredGraphics[layer].erase(graphic);
    }

    void Renderer::UnregisterAll()
    {
        for (int i = 0; i < numLayersActive; i++)
        {
            registeredGraphics[i].clear();
        }
    }

    void Renderer::ClearQueue()
    {
        for (int i = 0; i < numLayersActive; i++)
        {
            /// Queue created on stack
            queue<Graphic*> emptyQueue;
            /// Swap the data
            swap(queuedGraphics[i], emptyQueue);
            /// Swapped queue goes out of scope and memory is destroyed
        }
    }

    int Renderer::Enqueue(Graphic* graphic, int layer)
    {
        int intendedLayer = layer;
        layer = Clamp(layer, 0, numLayersActive);
        if (layer != intendedLayer)
        {
            Log.Warning("[Renderer] Enqueued graphic on layer [{0}] because the intended layer [{1}] is out of bounds (max layer is [{2}]).", layer, intendedLayer, numLayersActive - 1);
        }
        queuedGraphics[layer].push(graphic);
        return layer;
    }

    void Renderer::RenderPresent(bool manualMode)
    {
        if (!manualMode)
        {
            SDL_RenderClear(renderer);
        }
        for (int layer = 0; layer < numLayersActive; layer++)
        {
            for (auto i : registeredGraphics[layer])
            {
                i->Render(*this);
            }
            for (int i = 0, counti = queuedGraphics[layer].empty() ? 0 : queuedGraphics[layer].size(); i < counti; i++)
            {
                (queuedGraphics[layer].front())->Render(*this);
                queuedGraphics[layer].pop();
                #ifdef OSSIUM_DEBUG
                numRendered++;
                #endif // DEBUG
            }
        }
        #ifdef OSSIUM_DEBUG
        numRenderedPrevious = numRendered;
        numRendered = 0;
        #endif // DEBUG
        SetDrawColor(bufferColour);
        if (!manualMode)
        {
            SDL_RenderPresent(renderer);
        }
    }

    void Renderer::SetDrawColor(SDL_Color color)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }

    void Renderer::SetDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
    {
        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    }

    SDL_Color Renderer::GetDrawColor()
    {
        SDL_Color c;
        SDL_GetRenderDrawColor(renderer, &c.r, &c.g, &c.b, &c.a);
        return c;
    }

    void Renderer::SetBlendmode(SDL_BlendMode blending)
    {
        SDL_SetRenderDrawBlendMode(renderer, blending);
    }

    Window* Renderer::GetWindow()
    {
        return renderWindow;
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
        SDL_RenderSetViewport(renderer, &viewportRect);
    }

    void Renderer::WindowToViewportPoint(int& x, int& y)
    {
        y -= viewportRect.y;
        x -= viewportRect.x;
    }

    void Renderer::ResetViewport()
    {
        if (renderWindow == nullptr)
        {
            Log.Warning("Failed to reset renderer viewport! Associated window is NULL.");
            // Early out
            return;
        }
        SDL_Rect viewRect;
        float percent_width = 1.0f;
        float percent_height = 1.0f;
        int width = renderWindow->GetWidth();
        int height = renderWindow->GetHeight();
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
            viewRect.w = renderWindow->GetWidth();
            viewRect.h = renderWindow->GetHeight();
        }

        if (SDL_RenderSetViewport(renderer, &viewRect) < 0)
        {
            Log.Error("Failed to set viewport! SDL_Error: {0}", SDL_GetError());
        }

        viewportRect = viewRect;
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

    void Renderer::ReallocateLayers(int numLayers)
    {
        delete[] registeredGraphics;
        delete[] queuedGraphics;

        registeredGraphics = new set<Graphic*>[numLayers];
        queuedGraphics = new queue<Graphic*>[numLayers];

        numLayersActive = numLayers;
    }

    // GENERAL

    #ifdef OSSIUM_DEBUG
    int Renderer::GetNumRendered()
    {
        return numRenderedPrevious;
    }
    #endif // DEBUG

    int Renderer::GetNumLayers()
    {
        return numLayersActive;
    }

    SDL_Renderer* Renderer::GetRendererSDL()
    {
        return renderer;
    }

    SDL_Color Renderer::GetBackgroundColor()
    {
        return bufferColour;
    }

    void Renderer::SetBackgroundColor(SDL_Color color)
    {
        bufferColour = color;
    }

}
