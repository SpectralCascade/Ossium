#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL.h>

#include "callback.h"

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
        void HandleEvents(SDL_Event &event);

        /// Get/Set specifiers
        SDL_Window* GetWindow();
        int GetWidth();
        int GetHeight();
        int GetDisplayWidth();
        int GetDisplayHeight();
        void SetWidth(int newWidth);
        void SetHeight(int newHeight);
        void SetFullscreen();
        void SetWindowed();
        void SetBordered();
        void SetBorderless();

        /// Return active flags
        bool IsMinimised();
        bool IsFullscreen();
        bool IsFocus();

        /// Callbacks for window state changes
        Callback<Window> OnSizeChanged;
        Callback<Window> OnFullscreen;
        Callback<Window> OnWindowed;
        Callback<Window> OnMinimise;
        Callback<Window> OnMaximise;
        Callback<Window> OnFocusGained;
        Callback<Window> OnFocusLost;
        /// Called when the window is destroyed so any associated renderers can null their window pointers.
        Callback<Window> OnDestroyed;

    private:
        /// Prohibited copying of windows
        Window(const Window& src);
        Window operator=(const Window& src);

        /// Individual window instance
        SDL_Window* window;

        /// Window dimensions
        int width;
        int height;

        /// The dimensions of the main display
        int display_width;
        int display_height;

        /// Active flags
        bool minimized;
        bool fullscreen;
        bool focus;
        bool border;

    };

}

#endif // WINDOW_H
