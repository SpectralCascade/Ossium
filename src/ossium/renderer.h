#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <queue>

#include <SDL2/SDL.h>

#include "colours.h"
#include "vector.h"
#include "primitives.h"
#include "texture.h"
#include "window.h"
#include "text.h"

using namespace std;

namespace ossium
{

    class Texture;
    class Text;

    struct RenderInfoTexture
    {
        Texture* texture;
        SDL_Rect destRect;
        SDL_Rect srcRect;
    };

    struct RenderInfoTextureEx
    {
        Texture* texture;
        SDL_Rect destRect;
        SDL_Rect srcRect;
        SDL_Point origin;
        float angle;
        SDL_RendererFlip flip;
    };

    struct RenderInfoText
    {
        Text* text;
        SDL_Rect destRect;
        SDL_Rect srcRect;
    };

    struct RenderInfoTextEx
    {
        Text* text;
        SDL_Rect destRect;
        SDL_Rect srcRect;
        SDL_Point origin;
        float angle;
        SDL_RendererFlip flip;
    };

    struct RenderInfoPoint
    {
        SDL_Point p;
        SDL_Color colour;
    };

    struct RenderInfoLine
    {
        Line line;
        SDL_Color colour;
        SDL_RendererFlip flip;
    };

    struct RenderInfoRect
    {
        SDL_Rect rect;
        SDL_Color colour;
    };

    // Wrapper class for SDL_Renderer - also supports layering
    class Renderer
    {
    public:
        Renderer(Window* window, int numLayers = 1, bool culling = true, Uint32 flags = SDL_RENDERER_ACCELERATED, int driver = -1);
        ~Renderer();

        //
        // ENQUEUING METHODS
        // These methods add objects to their respective layer queue, ready for rendering
        //

        // Adds a texture pointer to a queue corresponding to the layer. If the layer does not exist, throws an exception
        void enqueue(Texture* texture, SDL_Rect dest, SDL_Rect src = {0, 0, 0, 0}, int layer = 0, bool forceCull = false);
        void enqueueEx(Texture* texture, SDL_Rect dest, SDL_Rect src = {0, 0, 0, 0}, int layer = 0, float angle = 0.0, SDL_Point origin = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE, bool forceCull = false);

        // Ditto but for fancy Text
        void enqueue(Text* text, SDL_Rect dest, SDL_Rect src = {0, 0, 0, 0}, int layer = 0, bool forceCull = false);
        void enqueueEx(Text* text, SDL_Rect dest, SDL_Rect src = {0, 0, 0, 0}, int layer = 0, float angle = 0, SDL_Point origin = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE, bool forceCull = false);

        // These all add a point, line or rect to the respective layer queue. If the layer does not exist, throws an exception
        void enqueue(SDL_Point* point, int layer = 0, SDL_Color colour = BLACK, bool forceCull = false);
        void enqueue(Line* line, int layer = 0, SDL_Color colour = BLACK, SDL_RendererFlip flip = SDL_FLIP_NONE, bool forceCull = false);
        void enqueue(SDL_Rect* rect, int layer = 0, bool filled = false, SDL_Color colour = BLACK, bool forceCull = false);

        //
        // RENDER METHODS
        // These methods render enqueued objects
        //

        // Renders a specific layer of textures, or all layers by default
        void renderTextures(int layer = -1);
        void renderTexturesEx(int layer = -1);

        // Ditto but for text
        void renderTexts(int layer = -1);
        void renderTextsEx(int layer = -1);

        // Renders a specific layer of points, lines and rects respectively, or all layers by default
        void renderPoints(int layer = -1);
        void renderLines(int layer = -1);
        void renderRects(int layer = -1);
        void renderFillRects(int layer = -1);
        // Ditto, but with offsets and rotations
        // Note: These methods are not yet supported
        /*void renderPointsEx(int layer = -1, int xOffset = 0, int yOffset = 0, SDL_Point* origin = NULL, float angle = 0.0);
        void renderLinesEx(int layer = -1, int xOffset = 0, int yOffset = 0, SDL_Point* origin = NULL, float angle = 0.0);
        void renderRectsEx(int layer = -1, int xOffset = 0, int yOffset = 0, SDL_Point* origin = NULL, float angle = 0.0);
        */
        // Renders EVERYTHING on a specified layer, or all layers by default
        // If the SPLIT argument is true, then ALL textures are rendered, followed by ALL fill rects, etc.
        // regardless of their actual layering
        void renderAll(int layer = -1, bool split = false);

        // Ditto but with options for x/y offsets and rotation
        // Note: not yet supported
        //void renderAllEx(int layer = -1, int xOffset = 0, int yOffset = 0, SDL_Point* origin = NULL, float angle = 0.0);

        // Clears whatever is currently sitting in the video/graphics memory buffer
        // This does NOT clear the layer queues - but they should be clear anyway when all layers are rendered
        void renderClear();

        // Calls SDL_RenderPresent(renderer);
        void renderPresent();

        //
        // GENERAL METHODS
        //

        // Turns render culling on or off
        // Culling is very crude at the moment - it simply performs brute-force intersect tests
        // when render objects are enqueued
        // Ideally, a spatial hash grid or a tree structure (e.g. quadtree) should be used so that only
        // render objects in cells that intersect the renderer are enqueued in the first place
        // This is only really here for convenience
        void setCulling(bool culling);

        // Returns the SDL renderer context associated with this renderer
        SDL_Renderer* getRenderer();

        // This triggers reallocation of memory for the layer queue arrays at the end of the next renderAll() call
        // Use this to add/remove layers dynamically at run time. Ideally, this will only be used when loading levels
        // (if at all)
        void changeLayers(int numLayers = 1);

        // Gets the number of culled and rendered objects respectively, in the previous frame
        int getNumCulled();
        int getNumRendered();

    private:
        // Copying of renderer is not permitted
        Renderer(const Renderer& thisCopy);
        Renderer operator=(const Renderer& thisCopy);

        // SDL renderer context that deals with the actual rendering
        SDL_Renderer* renderer;

        // Pointer to the window associated with this renderer, for getting width/height
        Window* renderWindow;

        // Number of layers this renderer has
        int numLayersActive;

        // When true, cull anything outside of the renderer view (this is applied when enqueuing takes place)
        bool renderCull;

        // These 2 metrics are handy for debugging/profiling
        // Number of objects culled in the current frame
        int numCulled;
        // Number of objects rendered in the current frame
        int numRendered;

        // Ditto, but for the previous frame
        int numCulledPrevious;
        int numRenderedPrevious;

        // Dynamic array of layer queues for various textures, where layer 0 is the topmost layer (rendered last of all)
        queue<RenderInfoTexture>* textureLayers;
        queue<RenderInfoTextureEx>* textureExLayers;

        // Ditto but for text
        queue<RenderInfoText>* textLayers;
        queue<RenderInfoTextEx>* textExLayers;

        // Dynamic arrays of layer queues for points, lines and rects
        queue<RenderInfoPoint>* pointLayers;
        queue<RenderInfoLine>* lineLayers;
        queue<RenderInfoRect>* rectLayers;
        queue<RenderInfoRect>* fillRectLayers;

    };

}

#endif // RENDERER_H
