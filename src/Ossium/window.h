#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL.h>

namespace Ossium
{
    /// Wrapper class for SDL_Window
    class Window
    {
    public:
        /// Appropriate constructor and destructor
        Window(const char* title = "Ossium Engine", int w = 640, int h = 480, bool fullscrn = false, Uint32 flags = SDL_WINDOW_SHOWN);
        ~Window();

        /// Handles window events
        void handle_events(SDL_Event &event);

        /// Get/Set specifiers
        SDL_Window* GetWindow();
        int GetWidth();
        int GetHeight();
        int GetAspectWidth();
        int GetAspectHeight();
        SDL_Rect GetViewportRect();
        void SetWidth(int newWidth);
        void SetHeight(int newHeight);
        void SetFullscreen();
        void SetWindowed();
        void SetBordered();
        void SetBorderless();
        /// Attempts to set the window's renderer viewport dimensions to the input target width and height in pixels
        /// If the window is bigger in both width and height than the target dimension arguments, the viewport will remain
        /// at the target dimensions and remaining space in the window will be filled with black bars on all sides.
        /// The letterbox parameter specifies whether the viewport should use black bars along the bottom and top
        /// if the window dimensions do not match the target aspect, or else the viewport will be cropped on the left and right side
        /// when the window dimensions do not match the target aspect.
        /// If fixed == true, the viewport matches the argument dimensions if window dimensions >= argument dimensions
        void SetAspectRatio(int aspect_w, int aspect_h, bool fixed = false);

        /// Updates the renderer viewport dimensions according to the current aspect
        void UpdateViewport();

        /// Return active flags
        bool IsMinimised();
        bool IsFullscreen();
        bool IsFocus();

    private:
        /// Prohibited copying of windows
        Window(const Window& src);
        Window operator=(const Window& src);

        /// Individual window instance
        SDL_Window* window;

        /// Window dimensions
        int width;
        int height;

        /// The maximum dimensions of the window
        int display_width;
        int display_height;

        /// Renderer viewport target width/height or aspect ratio
        int aspect_width;
        int aspect_height;
        /// Whether or not the renderer viewport uses a fixed aspect or dynamic aspect ratio
        bool fixed_aspect;
        /// Whether to use black bars or simply crop the left and right edges of the renderer to fit the target aspect
        bool letterbox_bars;
        /// The viewport rect, set when the viewport updates
        SDL_Rect viewportRect;

        /// Active flags
        bool minimized;
        bool fullscreen;
        bool focus;
        bool border;

    };

}

#endif // WINDOW_H
