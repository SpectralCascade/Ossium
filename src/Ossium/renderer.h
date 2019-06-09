#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <queue>
#include <set>

#include <SDL.h>

#include "colors.h"
#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    class Window;

    inline namespace Graphics
    {

        /// Forward declaration
        class Renderer;

        /// Anything that can be rendered should inherit from this interface and implement the Render method
        class Graphic
        {
        public:
            friend class Renderer;

        protected:
            virtual void Render(Renderer& renderer) = 0;

        };

        // Wrapper class for SDL_Renderer - also supports layering
        class Renderer
        {
        public:
            Renderer(Window* window, int numLayers = 1, Uint32 flags = SDL_RENDERER_ACCELERATED, int driver = -1);
            ~Renderer();

            /// Registers a graphic for rendering. Note that you cannot change the layer or forceCull parameter once registered;
            /// to do so you must unregister the graphic and re-register it with the modified arguments. Returns the layer used.
            int Register(Graphic* graphic, int layer = 0);
            /// Unregisters a graphic so it no longer renders.
            void Unregister(Graphic* graphic, int layer = 0);

            /// Unregisters all graphics
            void UnregisterAll();

            /// Clears the render queue
            void ClearQueue();

            /// Enqueues a graphic to be rendered in the next frame only. Returns the layer used.
            int Enqueue(Graphic* graphic, int layer = 0);

            /// Renders all registered and enqueued graphics in the current frame.
            /// Note that enqueued graphics are rendered last in each layer,
            /// so they appear on top of registered graphics if they overlap in the same layer.
            /// Set manualMode to true if you don't want to automatically clear the video buffer before drawing stuff
            /// and/or you wish to draw stuff after calling RenderPresent(). Note you will have to manually call
            /// SDL_RenderClear() and SDL_RenderPresent!
            void RenderPresent(bool manualMode = false);

            /// Sets the rendering color for drawn points, lines and rects
            void SetDrawColor(SDL_Color color);

            /// Overload takes individual values for convenience
            void SetDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 0xFF);

            /// Sets the renderer blend mode. By default the renderer is set to SDL_BLENDMODE_BLEND.
            void SetBlendmode(SDL_BlendMode blending);

            /// Returns a pointer to the window associated with this renderer
            Window* GetWindow();

            /// Returns the viewport width of this renderer
            int GetWidth();
            /// Returns the viewport height of this renderer
            int GetHeight();

            /// Returns the aspect width
            int GetAspectWidth();
            /// Returns the aspect height
            int GetAspectHeight();
            /// Returns the viewport area used for rendering
            SDL_Rect GetViewportRect();

            /// Attempts to set the viewport dimensions to the input target width and height in pixels
            /// If the window is bigger in both width and height than the target dimension arguments, the viewport will remain
            /// at the target dimensions and remaining space in the window will be filled with black bars on all sides.
            /// The letterbox parameter specifies whether the viewport should use black bars along the bottom and top
            /// if the window dimensions do not match the target aspect, or else the viewport will be cropped on the left and right side
            /// when the window dimensions do not match the target aspect.
            /// If fixed == true, the viewport matches the argument dimensions if window dimensions >= argument dimensions
            void SetAspectRatio(int aspect_w, int aspect_h, bool fixed = false);

            /// Renders a specific layer of graphics; renders everything if layer < 0
            void RenderLayer(int layer = -1);

            /// Returns the SDL renderer context associated with this renderer
            SDL_Renderer* GetRendererSDL();

            /// This does what it says on the tin. Ideally, this will only be used when loading levels
            /// (if at all). Note that enqueued and registered graphics are removed and unregistered upon calling this method!
            void ReallocateLayers(int numLayers);

            #ifdef DEBUG
            /// Returns the number of rendered graphics in the previous frame
            int GetNumRendered();
            #endif // DEBUG

        private:
            NOCOPY(Renderer);

            /// Deals with the actual rendering
            SDL_Renderer* renderer;

            /// Pointer to the window associated with this renderer
            Window* renderWindow;

            /// Updates the renderer viewport dimensions according to the current aspect. Called when the window changes
            /// size switches between fullscreen and windowed modes.
            void UpdateViewport(Window& windowCaller);

            /// Called when the associated window is destroyed.
            void OnWindowDestroyed(Window& windowCaller);

            /// Renderer viewport target width/height or aspect ratio
            int aspect_width;
            int aspect_height;
            /// Whether or not the renderer viewport uses a fixed aspect or dynamic aspect ratio
            bool fixed_aspect;
            /// Whether to use black bars or simply crop the left and right edges of the renderer to fit the target aspect
            bool letterbox_bars;
            /// The viewport rect, set when the viewport updates
            SDL_Rect viewportRect;

            /// Cached array of callback ids to unregister this renderer when destroyed.
            int callbackIds[3] = {0, 0, 0};

            /// Number of layers this renderer has
            int numLayersActive;

            #ifdef DEBUG
            /// Number of graphics rendered in the current frame
            int numRendered;
            /// Number of graphics rendered in the previous frame
            int numRenderedPrevious;
            #endif // DEBUG

            /// Registered graphics
            set<Graphic*>* registeredGraphics;
            /// Graphics queued to be rendered for this frame only
            queue<Graphic*>* queuedGraphics;

        };

    }

}

#endif // RENDERER_H
