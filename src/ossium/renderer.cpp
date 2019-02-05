#include <queue>

#include <SDL.h>

#include "colours.h"
#include "primitives.h"
#include "vector.h"
#include "texture.h"
#include "window.h"
#include "text.h"
#include "renderer.h"

using namespace std;

namespace Ossium
{

    inline namespace graphics
    {

        Renderer::Renderer(Window* window, int numLayers, bool culling, Uint32 flags, int driver)
        {
            #ifdef DEBUG
            SDL_assert(window != NULL);
            #endif // DEBUG
            renderer = SDL_CreateRenderer(window->getWindow(), driver, flags);
            if (renderer == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer! SDL_Error: %s", SDL_GetError());
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
                renderer = SDL_CreateRenderer(window->getWindow(), driver, SDL_RENDERER_SOFTWARE);
                if (renderer == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Fallback software renderer could not be created! SDL_Error: %s", SDL_GetError());
                }
            }
            textureLayers = new queue<internals::RenderInfoTexture>[numLayers];
            textureExLayers = new queue<internals::RenderInfoTextureEx>[numLayers];
            textLayers = new queue<internals::RenderInfoText>[numLayers];
            textExLayers = new queue<internals::RenderInfoTextEx>[numLayers];
            pointLayers = new queue<internals::RenderInfoPoint>[numLayers];
            lineLayers = new queue<internals::RenderInfoLine>[numLayers];
            rectLayers = new queue<internals::RenderInfoRect>[numLayers];
            fillRectLayers = new queue<internals::RenderInfoRect>[numLayers];
            renderCull = culling;
            numLayersActive = numLayers;
            renderWindow = window;
        }

        Renderer::~Renderer()
        {
            SDL_RenderClear(renderer);
            SDL_DestroyRenderer(renderer);
            renderer = NULL;
            delete[] textureLayers;
            delete[] textureExLayers;
            delete[] textLayers;
            delete[] textExLayers;
            delete[] pointLayers;
            delete[] lineLayers;
            delete[] rectLayers;
            delete[] fillRectLayers;
        }

        // ENQUEUING

        void Renderer::enqueue(Texture* texture, SDL_Rect dest, SDL_Rect src, int layer, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                // Check if the destination rect is inside or outside the renderer view
                if (!IntersectSDL(dest, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    // Cull this object by simply not enqueuing it for rendering
                    numCulled++;
                    return;
                }
            }
            textureLayers[layer].push((internals::RenderInfoTexture){texture, dest, src});
        }

        void Renderer::enqueueEx(Texture* texture, SDL_Rect dest, SDL_Rect src, int layer, float angle, SDL_Point origin, SDL_RendererFlip flip, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                // Check if the destination rect is inside or outside the renderer view
                if (!IntersectSDL(dest, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    // Cull this object by simply not enqueuing it for rendering
                    numCulled++;
                    return;
                }
            }
            textureExLayers[layer].push((internals::RenderInfoTextureEx){texture, dest, src, origin, angle, flip});
        }

        void Renderer::enqueue(Text* text, SDL_Rect dest, SDL_Rect src, int layer, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                // Check if the destination rect is inside or outside the renderer view
                if (!IntersectSDL(dest, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    // Cull this object by simply not enqueuing it for rendering
                    numCulled++;
                    return;
                }
            }
            textLayers[layer].push((internals::RenderInfoText){text, dest, src});
        }
        void Renderer::enqueueEx(Text* text, SDL_Rect dest, SDL_Rect src, int layer, float angle, SDL_Point origin, SDL_RendererFlip flip, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                // Check if the destination rect is inside or outside the renderer view
                if (!IntersectSDL(dest, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    // Cull this object by simply not enqueuing it for rendering
                    numCulled++;
                    return;
                }
            }
            textExLayers[layer].push((internals::RenderInfoTextEx){text, dest, src, origin, angle, flip});
        }

        void Renderer::enqueue(SDL_Point* point, int layer, SDL_Color colour, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                if (!IntersectSDL(*point, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    numCulled++;
                    return;
                }
            }
            pointLayers[layer].push((internals::RenderInfoPoint){*point, colour});
        }

        void Renderer::enqueue(Line* line, int layer, SDL_Color colour, SDL_RendererFlip flip, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                if (!Intersect(line->p, (Rectangle){0, 0, (float)renderWindow->getWidth(), (float)renderWindow->getHeight()}) && !Intersect(line->u, (Rectangle){0, 0, (float)renderWindow->getWidth(), (float)renderWindow->getHeight()}))
                {
                    numCulled++;
                    return;
                }
            }
            lineLayers[layer].push((internals::RenderInfoLine){*line, colour, flip});
        }

        void Renderer::enqueue(SDL_Rect* rect, int layer, bool filled, SDL_Color colour, bool forceCull)
        {
            #ifdef DEBUG
            SDL_assert(layer >= 0);
            #endif // DEBUG
            if (renderCull || forceCull)
            {
                if (!IntersectSDL(*rect, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
                {
                    numCulled++;
                    return;
                }
            }
            if (filled)
            {
                fillRectLayers[layer].push((internals::RenderInfoRect){*rect, colour});
            }
            else
            {
                rectLayers[layer].push((internals::RenderInfoRect){*rect, colour});
            }
        }

        // RENDERING

        void Renderer::renderTextures(int layer)
        {
            if (layer < 0)
            {
                // Iterate through all layers
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (textureLayers[i].empty())
                    {
                        continue;
                    }
                    // Iterate through queue
                    int rendered = textureLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoTexture info = textureLayers[i].front();
                        textureLayers[i].pop();
                        info.texture->renderTextureSimple(renderer, info.destRect, &info.srcRect);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (textureLayers[layer].empty())
                {
                    return;
                }
                int rendered = textureLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoTexture info = textureLayers[layer].front();
                    textureLayers[layer].pop();
                    info.texture->renderTextureSimple(renderer, info.destRect, &info.srcRect);
                }
                numRendered += rendered;
            }
        }

        void Renderer::renderTexturesEx(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive  -1; i >= 0; i--)
                {
                    if (textureExLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = textureExLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoTextureEx info = textureExLayers[i].front();
                        textureExLayers[i].pop();
                        info.texture->renderTexture(renderer, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (textureExLayers[layer].empty())
                {
                    return;
                }
                int rendered = textureExLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoTextureEx info = textureExLayers[layer].front();
                    textureExLayers[layer].pop();
                    info.texture->renderTexture(renderer, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
                }
            }
        }

        void Renderer::renderTexts(int layer)
        {
            if (layer < 0)
            {
                // Iterate through all layers
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (textLayers[i].empty())
                    {
                        continue;
                    }
                    // Iterate through queue
                    int rendered = textLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoText info = textLayers[i].front();
                        textLayers[i].pop();
                        info.text->renderTextSimple(this, info.destRect, &info.srcRect);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (textLayers[layer].empty())
                {
                    return;
                }
                int rendered = textLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoText info = textLayers[layer].front();
                    textLayers[layer].pop();
                    info.text->renderTextSimple(this, info.destRect, &info.srcRect);
                }
                numRendered += rendered;
            }
        }
        void Renderer::renderTextsEx(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive  -1; i >= 0; i--)
                {
                    if (textExLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = textExLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoTextEx info = textExLayers[i].front();
                        textExLayers[i].pop();
                        info.text->renderText(this, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (textExLayers[layer].empty())
                {
                    return;
                }
                int rendered = textExLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoTextEx info = textExLayers[layer].front();
                    textExLayers[layer].pop();
                    info.text->renderText(this, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
                }
            }
        }

        void Renderer::renderPoints(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (pointLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = pointLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoPoint info = pointLayers[i].front();
                        pointLayers[i].pop();
                        SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                        SDL_RenderDrawPoint(renderer, info.p.x, info.p.y);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (pointLayers[layer].empty())
                {
                    return;
                }
                int rendered = pointLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoPoint info = pointLayers[layer].front();
                    pointLayers[layer].pop();
                    SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                    SDL_RenderDrawPoint(renderer, info.p.x, info.p.y);
                }
                numRendered += rendered;
            }
        }

        void Renderer::renderLines(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (lineLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = lineLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoLine info = lineLayers[i].front();
                        lineLayers[i].pop();
                        SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                        SDL_RenderDrawLine(renderer, (int)info.line.p.x, (int)info.line.p.y, (int)info.line.u.x, (int)info.line.p.y);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (lineLayers[layer].empty())
                {
                    return;
                }
                int rendered = lineLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoLine info = lineLayers[layer].front();
                    lineLayers[layer].pop();
                    SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                    SDL_RenderDrawLine(renderer, (int)info.line.p.x, (int)info.line.p.y, (int)info.line.u.x, (int)info.line.p.y);
                }
                numRendered += rendered;
            }
        }

        void Renderer::renderRects(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (rectLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = rectLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoRect info = rectLayers[i].front();
                        rectLayers[i].pop();
                        SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                        SDL_RenderFillRect(renderer, &info.rect);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (rectLayers[layer].empty())
                {
                    return;
                }
                int rendered = rectLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoRect info = rectLayers[layer].front();
                    rectLayers[layer].pop();
                    SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                    SDL_RenderDrawRect(renderer, &info.rect);
                }
                numRendered += rendered;
            }
        }

        void Renderer::renderFillRects(int layer)
        {
            if (layer < 0)
            {
                for (int i = numLayersActive - 1; i >= 0; i--)
                {
                    if (fillRectLayers[i].empty())
                    {
                        continue;
                    }
                    int rendered = fillRectLayers[i].size();
                    for (int j = 0; j < rendered; j++)
                    {
                        internals::RenderInfoRect info = fillRectLayers[i].front();
                        fillRectLayers[i].pop();
                        SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                        SDL_RenderFillRect(renderer, &info.rect);
                    }
                    numRendered += rendered;
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                if (fillRectLayers[layer].empty())
                {
                    return;
                }
                int rendered = fillRectLayers[layer].size();
                for (int i = 0; i < rendered; i++)
                {
                    internals::RenderInfoRect info = fillRectLayers[layer].front();
                    fillRectLayers[layer].pop();
                    SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                    SDL_RenderFillRect(renderer, &info.rect);
                }
                numRendered += rendered;
            }
        }

        void Renderer::renderAll(int layer, bool split)
        {
            if (layer < 0)
            {
                if (split)
                {
                    renderTextures(-1);
                    renderTexts(-1);
                    renderFillRects(-1);
                    renderRects(-1);
                    renderLines(-1);
                    renderPoints(-1);
                }
                else
                {
                    // Reverse iterate through layers so that they are rendered with layer 0 at the top
                    for (int i = numLayersActive - 1; i >= 0; i--)
                    {
                        renderTextures(i);
                        renderTexts(i);
                        renderFillRects(i);
                        renderRects(i);
                        renderLines(i);
                        renderPoints(i);
                    }
                }
            }
            else
            {
                #ifdef DEBUG
                SDL_assert(layer < numLayersActive);
                #endif // DEBUG
                renderTextures(layer);
                renderTexts(layer);
                renderFillRects(layer);
                renderRects(layer);
                renderLines(layer);
                renderPoints(layer);
            }
            /// Set background draw colour to black
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        }

        void Renderer::renderPresent()
        {
            SDL_RenderPresent(renderer);
        }

        void Renderer::renderClear()
        {
            SDL_RenderClear(renderer);
            numRenderedPrevious = numRendered;
            numCulledPrevious = numCulled;
            numRendered = 0;
            numCulled = 0;
        }

        // GENERAL

        int Renderer::getNumCulled()
        {
            return numCulledPrevious;
        }
        int Renderer::getNumRendered()
        {
            return numRenderedPrevious;
        }

        SDL_Renderer* Renderer::getRenderer()
        {
            return renderer;
        }

    }

}
