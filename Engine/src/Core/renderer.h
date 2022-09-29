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
#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <queue>
#include <set>
extern "C"
{
    #include <SDL.h>
}
#include "bgfx/bgfx.h"

#include "colors.h"
#include "helpermacros.h"
#include "services.h"
#include "renderinput.h"
#include "renderview.h"
#include "rendertarget.h"

namespace Ossium
{
    // Forward declarations

    class Renderer;

    // Anything that can be rendered should inherit from this interface and implement the Render method
    class OSSIUM_EDL Graphic
    {
    public:
        friend class Renderer;

    protected:
        virtual void Render(Renderer& renderer) = 0;

    };

    // Wrapper class for SDL_Renderer - also supports layering
    class OSSIUM_EDL Renderer : public Service<Renderer>
    {
    private:
        // Attempt to initialise bgfx
        bool InitBGFX();

    public:
        // Create a renderer.
        Renderer(RenderTarget* renderTarget, RenderViewPool* renderViewPool, int numLayers = 1);
        virtual ~Renderer();

        // Registers a graphic for rendering. Note that you cannot change the layer or forceCull parameter once registered;
        // to do so you must unregister the graphic and re-register it with the modified arguments. Returns the layer used.
        int Register(Graphic* graphic, int layer = 0);
        // Unregisters a graphic so it no longer renders.
        void Unregister(Graphic* graphic, int layer = 0);

        // Unregisters all graphics
        void UnregisterAll();

        // Clears the render queue
        void ClearQueue();

        // Enqueues a graphic to be rendered in the next frame only. Returns the layer used.
        int Enqueue(Graphic* graphic, int layer = 0);

        // Renders all registered and enqueued graphics in the current frame.
        // Note that enqueued graphics are rendered last in each layer,
        // so they appear on top of registered graphics if they overlap in the same layer.
        // Set manualMode to true if you don't want to automatically clear the video buffer before drawing stuff
        // and/or you wish to draw stuff after calling RenderPresent(). Note you will have to manually call
        // SDL_RenderClear() and SDL_RenderPresent!
        void RenderPresent(bool manualMode = false);

        // Sets the default rendering color.
        void SetDrawColor(SDL_Color color);

        // Overload takes individual values for convenience
        void SetDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 0xFF);

        // Returns the current draw color
        SDL_Color GetDrawColor();

        // Returns the current draw color as a Uint32
        Uint32 GetDrawColorUint32();

        // Set the bgfx state for this renderer
        void SetState(uint64_t state);

        // Returns the bgfx state for this renderer
        uint64_t GetState();

        // Updates the bgfx state and draw colour.
        void UpdateStateAndColor();

        // Returns a pointer to the render target.
        RenderTarget* GetTarget();

        // Returns the viewport width of this renderer
        int GetWidth();
        // Returns the viewport height of this renderer
        int GetHeight();

        // Returns the aspect width
        int GetAspectWidth();
        // Returns the aspect height
        int GetAspectHeight();

        // Returns the viewport area used for rendering
        SDL_Rect GetViewportRect();

        // Sets the viewport area used for rendering
        void SetViewportRect(SDL_Rect rect);

        // Converts a position relative to the render target into a position relative to the view port.
        void TargetToViewportPoint(int& x, int& y);

        // Attempts to set the viewport dimensions to the input target width and height in pixels
        /**
            If the render target is bigger in both width and height than the target dimension arguments, the viewport will remain
            at the render target dimensions and remaining space in the render target will be filled with black bars on all sides.
            The letterbox parameter specifies whether the viewport should use black bars along the bottom and top
            if the render target dimensions do not match the target aspect, or else the viewport will be cropped on the left and right side
            when the render target dimensions do not match the target aspect.
            If fixed == true, the viewport matches the argument dimensions if render target dimensions >= argument dimensions
            If resetViewport == false, the viewport is not automatically reset.
        **/
        void SetAspectRatio(int aspect_w, int aspect_h, bool fixed = false, bool resetViewport = true);

        // Renders a specific layer of graphics; renders everything if layer < 0
        void RenderLayer(int layer = -1);

        // This does what it says on the tin. Ideally, this will only be used when loading levels
        // (if at all). Note that enqueued and registered graphics are removed and unregistered upon calling this method!
        void ReallocateLayers(int numLayers);

        #ifdef OSSIUM_DEBUG
        // Returns the number of rendered graphics in the previous frame
        int GetNumRendered();
        #endif // DEBUG

        // Returns the number of layers allocated by the renderer
        int GetNumLayers();

        // Returns the render clear colour.
        SDL_Color GetBackgroundColor();

        // Sets the render clear colour.
        void SetBackgroundColor(SDL_Color color);

        // Updates the renderer viewport dimensions according to the current aspect of the render target.
        void ResetViewport();

        // Add an input to render.
        // Each input is rendered one after the other in passes; so for example, adding a geometry buffer
        // and a shadow map texture followed by lighting would first render geometry,
        // then render the shadow map, then render lighting to the target.
        void AddInput(RenderInput* input);
        
        // Remove an input that no longer needs to be rendered.
        void RemoveInput(RenderInput* input);
        
    private:
        NOCOPY(Renderer);

        // The target that is rendered to
        RenderTarget* target;

        // The pool of render views available for use
        RenderViewPool* renderViewPool;

        // Inputs for rendering (render passes)
        std::vector<RenderInput*> inputs;
        
        // Renderer viewport target width/height or aspect ratio
        int aspect_width = 0;
        int aspect_height = 0;
        // Whether or not the renderer viewport uses a fixed aspect or dynamic aspect ratio
        bool fixed_aspect = false;
        // Whether to use black bars or simply crop the left and right edges of the renderer to fit the target aspect
        bool letterbox_bars = true;
        // The viewport rect, set when the viewport updates
        SDL_Rect viewportRect;

        // The render clear colour, defaults to black.
        SDL_Color bufferColour = Colors::BLACK;

        // The render draw colour, defaults to white.
        SDL_Color drawColour = Colors::WHITE;

        // The bgfx state to use for this renderer, defaults to BGFX_STATE_DEFAULT.
        uint64_t state = BGFX_STATE_DEFAULT;

        /// Number of layers this renderer has
        int numLayersActive;

        #ifdef OSSIUM_DEBUG
        /// Number of graphics rendered in the current frame
        int numRendered;
        /// Number of graphics rendered in the previous frame
        int numRenderedPrevious;
        #endif // DEBUG

        /// Registered graphics
        std::set<Graphic*>* registeredGraphics;
        /// Graphics queued to be rendered for this frame only
        std::queue<Graphic*>* queuedGraphics;

    };

    // Defines a simple position vertex with a colour
    struct PositionVertex
    {
        float x;
        float y;
        float z;
        uint32_t abgr;
    };

}

#endif // RENDERER_H
