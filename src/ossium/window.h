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
        Window(const char* title = "Ossium Engine", int width = 640, int height = 480, bool fullscrn = false);
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

        /// Return active flags
        bool isMinimized();
        bool isFullscreen();
        bool isFocus();

    private:
        /// Prohibited copying of windows
        Window(const Window& thisCopy);
        Window operator=(const Window& thisCopy);

        /// Individual window instance
        SDL_Window* window;

        /// Window dimensions
        int width;
        int height;

        /// Active flags
        bool minimized;
        bool fullscreen;
        bool focus;
        bool border;

    };

}

#endif // WINDOW_H
