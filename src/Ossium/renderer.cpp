#include <queue>
#include <algorithm>
#include <unordered_set>

#include <SDL.h>

#include "renderer.h"
#include "window.h"

using namespace std;

namespace Ossium
{

    inline namespace graphics
    {

        Renderer::Renderer(Window* window, int numLayers, Uint32 flags, int driver)
        {
            #ifdef DEBUG
            SDL_assert(window != NULL);
            #endif // DEBUG
            renderer = SDL_CreateRenderer(window->GetWindow(), driver, flags);
            if (renderer == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer] Could not create renderer! SDL_Error: %s", SDL_GetError());
                int n_drivers = SDL_GetNumRenderDrivers();
                SDL_RendererInfo driver_data;
                string drivers_available;
                for (int i = 0; i < n_drivers; i++)
                {
                    SDL_GetRenderDriverInfo(i, &driver_data);
                    drivers_available = drivers_available + driver_data.name + ", ";
                }
                SDL_Log("Available render drivers are: %s", drivers_available.c_str());
                SDL_Log("Falling back to software renderer by default.");
                renderer = SDL_CreateRenderer(window->GetWindow(), driver, SDL_RENDERER_SOFTWARE);
                if (renderer == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer] Fallback software renderer could not be created! SDL_Error: %s", SDL_GetError());
                }
            }
            if (renderer != NULL)
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            }
            registeredGraphics = new set<Graphic*>[numLayers];
            queuedGraphics = new queue<Graphic*>[numLayers];

            numLayersActive = numLayers;
            renderWindow = window;
        }

        Renderer::~Renderer()
        {
            SDL_RenderClear(renderer);
            SDL_DestroyRenderer(renderer);
            renderer = NULL;

            delete[] registeredGraphics;
            delete[] queuedGraphics;
        }

        int Renderer::Register(Graphic* graphic, int layer)
        {
            int intendedLayer = layer;
            layer = clamp(layer, 0, numLayersActive);
            if (layer != intendedLayer)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "[Renderer] Registered graphic on layer [%d] because the intended layer [%d] is out of bounds (max layer is [%d]).", layer, intendedLayer, numLayersActive - 1);
            }
            registeredGraphics[layer].insert(graphic);
            return layer;
        }

        void Renderer::Unregister(Graphic* graphic, int layer)
        {
            if (!(layer >= 0 && layer < numLayersActive))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "[Renderer] Failed to unregister a graphic because the intended layer [%d] is out of bounds (max layer is %d).", layer, numLayersActive - 1);
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
            layer = clamp(layer, 0, numLayersActive);
            if (layer != intendedLayer)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "[Renderer] Enqueued graphic on layer [%d] because the intended layer [%d] is out of bounds (max layer is [%d]).", layer, intendedLayer, numLayersActive - 1);
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
                    #ifdef DEBUG
                    numRendered++;
                    #endif // DEBUG
                }
            }
            #ifdef DEBUG
            numRenderedPrevious = numRendered;
            numRendered = 0;
            #endif // DEBUG
            if (!manualMode)
            {
                /// Doing this ensures the background is always black
                SetDrawColour(colours::BLACK);
                SDL_RenderPresent(renderer);
            }
        }

        void Renderer::SetDrawColour(SDL_Color colour)
        {
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
        }

        void Renderer::SetDrawColour(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
        {
            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
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
            SDL_Rect view = renderWindow != nullptr ? renderWindow->GetViewportRect() : (SDL_Rect){0, 0, 0, 0};
            return view.w;
        }
        /// Returns the viewport height of this renderer
        int Renderer::GetHeight()
        {
            SDL_Rect view = renderWindow != nullptr ? renderWindow->GetViewportRect() : (SDL_Rect){0, 0, 0, 0};
            return view.h;
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

        #ifdef DEBUG
        int Renderer::GetNumRendered()
        {
            return numRenderedPrevious;
        }
        #endif // DEBUG

        SDL_Renderer* Renderer::GetRendererSDL()
        {
            return renderer;
        }

    }

}
