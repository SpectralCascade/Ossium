#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL2/SDL.h>

namespace ossium
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
        SDL_Window* getWindow();
        int getWidth();
        int getHeight();
        void setWidth(int newWidth);
        void setHeight(int newHeight);
        void setFullScreen();
        void setWindowed();
        void setBorder(SDL_bool bordered);
        /// Attempts to set the window's renderer viewport dimensions to the input target width and height in pixels
        /// If the window is bigger in both width and height than the target dimension arguments, the viewport will remain
        /// at the target dimensions and remaining space in the window will be filled with black bars on all sides.
        /// The letterbox parameter specifies whether the viewport should use black bars along the bottom and top
        /// if the window dimensions do not match the target aspect, or else the viewport will be cropped on the left and right side
        /// when the window dimensions do not match the target aspect.
        /// If fixed == true, the viewport matches the argument dimensions if window dimensions >= argument dimensions
        void setAspectRatio(int aspect_w, int aspect_h, bool fixed = false, bool letterbox = true);

        /// Updates the renderer viewport dimensions according to the current aspect
        void updateViewport();

        /// Return active flags
        bool isMinimized();
        bool isFullscreen();
        bool isFocus();

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

        /// Active flags
        bool minimized;
        bool fullscreen;
        bool focus;
        bool border;

    };

}

#endif // WINDOW_H
