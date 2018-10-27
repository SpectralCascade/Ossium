#include <queue>

#include <SDL2/SDL.h>

#include "colours.h"
#include "primitives.h"
#include "vector.h"
#include "texture.h"
#include "window.h"
#include "renderer.h"

using namespace std;

namespace ossium
{
    Renderer::Renderer(Window* window, int numLayers, bool culling, Uint32 flags, int driver)
    {
        SDL_assert(window != NULL);
        renderer = SDL_CreateRenderer(window->getWindow(), driver, flags);
        textureLayers = new queue<RenderInfoTexture>[numLayers];
        textureExLayers = new queue<RenderInfoTextureEx>[numLayers];
        pointLayers = new queue<RenderInfoPoint>[numLayers];
        lineLayers = new queue<RenderInfoLine>[numLayers];
        rectLayers = new queue<RenderInfoRect>[numLayers];
        fillRectLayers = new queue<RenderInfoRect>[numLayers];
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
        textureLayers[layer].push((RenderInfoTexture){texture, dest, src});
    }

    void Renderer::enqueueEx(Texture* texture, SDL_Rect dest, SDL_Rect src, int layer, SDL_Point origin, float angle, SDL_RendererFlip flip, bool forceCull)
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
        textureExLayers[layer].push((RenderInfoTextureEx){texture, dest, src, origin, angle, flip});
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
        pointLayers[layer].push((RenderInfoPoint){*point, colour});
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
        lineLayers[layer].push((RenderInfoLine){*line, colour, flip});
    }

    void Renderer::enqueue(SDL_Rect* rect, int layer, bool filled, SDL_Color colour)
    {
        #ifdef DEBUG
        SDL_assert(layer >= 0);
        #endif // DEBUG
        if (renderCull)
        {
            if (!IntersectSDL(*rect, {0, 0, renderWindow->getWidth(), renderWindow->getHeight()}))
            {
                numCulled++;
                return;
            }
        }
        if (filled)
        {
            fillRectLayers[layer].push((RenderInfoRect){*rect, colour});
        }
        else
        {
            rectLayers[layer].push((RenderInfoRect){*rect, colour});
        }
    }

    // RENDERING

    void Renderer::renderTextures(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == textureLayers->size());
        #endif
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
                    RenderInfoTexture info = textureLayers[i].front();
                    textureLayers[i].pop();
                    info.texture->renderSimple(renderer, info.destRect, &info.srcRect);
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
                RenderInfoTexture info = textureLayers[layer].front();
                textureLayers[layer].pop();
                info.texture->renderSimple(renderer, info.destRect, &info.srcRect);
            }
            numRendered += rendered;
        }
    }

    void Renderer::renderTexturesEx(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == textureExLayers->size());
        #endif
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
                    RenderInfoTextureEx info = textureExLayers[i].front();
                    textureExLayers[i].pop();
                    info.texture->render(renderer, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
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
                RenderInfoTextureEx info = textureExLayers[layer].front();
                textureExLayers[layer].pop();
                info.texture->render(renderer, info.destRect, &info.srcRect, info.angle, &info.origin, info.flip);
            }
        }
    }

    void Renderer::renderPoints(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == pointLayers->size());
        #endif
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
                    RenderInfoPoint info = pointLayers[i].front();
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
                RenderInfoPoint info = pointLayers[layer].front();
                pointLayers[layer].pop();
                SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                SDL_RenderDrawPoint(renderer, info.p.x, info.p.y);
            }
            numRendered += rendered;
        }
    }

    void Renderer::renderLines(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == lineLayers->size());
        #endif
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
                    RenderInfoLine info = lineLayers[i].front();
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
                RenderInfoLine info = lineLayers[layer].front();
                lineLayers[layer].pop();
                SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                SDL_RenderDrawLine(renderer, (int)info.line.p.x, (int)info.line.p.y, (int)info.line.u.x, (int)info.line.p.y);
            }
            numRendered += rendered;
        }
    }

    void Renderer::renderRects(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == rectLayers->size());
        #endif
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
                    RenderInfoRect info = rectLayers[i].front();
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
                RenderInfoRect info = rectLayers[layer].front();
                rectLayers[layer].pop();
                SDL_SetRenderDrawColor(renderer, info.colour.r, info.colour.g, info.colour.b, info.colour.a);
                SDL_RenderDrawRect(renderer, &info.rect);
            }
            numRendered += rendered;
        }
    }

    void Renderer::renderFillRects(int layer)
    {
        #ifdef DEBUG
        SDL_assert((unsigned int)numLayersActive == fillRectLayers->size());
        #endif
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
                    RenderInfoRect info = fillRectLayers[i].front();
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
                RenderInfoRect info = fillRectLayers[layer].front();
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
            // Reverse iterate through layers so that they are rendered with layer 0 at the top
            for (int i = numLayersActive - 1; i >= 0; i--)
            {
                renderTextures(i);
                renderFillRects(i);
                renderRects(i);
                renderLines(i);
                renderPoints(i);
            }
        }
        else
        {
            #ifdef DEBUG
            SDL_assert(layer < numLayersActive);
            #endif // DEBUG
            renderTextures(layer);
            renderFillRects(layer);
            renderRects(layer);
            renderLines(layer);
            renderPoints(layer);
        }
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
