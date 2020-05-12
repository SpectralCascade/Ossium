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
#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <set>
extern "C"
{
    #include <SDL2/SDL.h>
}

#include "services.h"
#include "callback.h"

using namespace std;

namespace Ossium
{

    /// Wrapper class for SDL_Window
    class OSSIUM_EDL Window
    {
    public:
        Window(const char* title = "Ossium Engine", int w = 640, int h = 480, bool fullscrn = false, Uint32 flags = SDL_WINDOW_SHOWN);
        ~Window();

        /// Handles window events
        void HandleEvent(SDL_Event &event);

        // Get/Set specifiers
        /// Returns a pointer to the SDL instance representing the native window.
        SDL_Window* GetWindowSDL();
        /// Returns the width of this window.
        int GetWidth();
        /// Returns the height of this window.
        int GetHeight();
        /// Returns the total height of the display.
        int GetDisplayWidth();
        /// Returns the total width of the display.
        int GetDisplayHeight();
        /// Returns the title of this window.
        string GetTitle();

        /// Sets the width of this window.
        void SetWidth(int newWidth);
        /// Sets the height of this window.
        void SetHeight(int newHeight);
        /// Switches this window from windowed to fullscreen mode.
        void SetFullscreen();
        /// Switches this window from fullscreen to windowed mode.
        void SetWindowed();
        /// Makes this window bordered.
        void SetBordered();
        /// Makes this window borderless.
        void SetBorderless();
        /// Sets the title of this window.
        void SetTitle(string title);

        // Return active flags
        /// Is this window minimised?
        bool IsMinimised();
        /// Is this window fullscreen?
        bool IsFullscreen();
        /// Is this the window the user is currently interacting with?
        bool IsFocus();
        /// Is the mouse cursor over the window?
        bool IsMouseFocus();

        // Callbacks for window state changes
        /// Called when the window size has been changed; this includes cases the size is changed directly by the API.
        Callback<Window> OnSizeChanged;
        /// Called when the window has been resized; this is preceded by OnSizeChanged and only gets called if the change was made by the user or native window manager.
        Callback<Window> OnResize;
        /// Called when the window switches to fullscreen.
        Callback<Window> OnFullscreen;
        /// Called when the window switches from fullscreen back to normal.
        Callback<Window> OnWindowed;
        /// Called when the window is minimised.
        Callback<Window> OnMinimise;
        /// Called when the window is maxinised.
        Callback<Window> OnMaximise;
        /// Called when the user interacts with the window while it is not in focus.
        Callback<Window> OnFocusGained;
        /// Called when the user interacts with something that isn't part of this window.
        Callback<Window> OnFocusLost;
        /// Called when the mouse cursor touches the window when it has not been touching it.
        Callback<Window> OnMouseEnter;
        /// Called when the mouse cursor leaves the window when it had been touching it previously.
        Callback<Window> OnMouseLeave;
        /// Called when the close button is used.
        Callback<Window> OnCloseButton;
        /// Called when the window is destroyed so any associated renderers can null their window pointers.
        Callback<Window> OnDestroyed;

    private:
        // Prohibited copying of windows
        Window(const Window& src);
        Window operator=(const Window& src);

        // Individual window instance
        SDL_Window* window;

        // Window dimensions
        int width;
        int height;

        // The dimensions of the main display
        int display_width;
        int display_height;

        // Active flags
        bool minimized;
        bool fullscreen;
        bool focus;
        bool mouseFocus;
        bool border;

    };

/*
    class OSSIUM_EDL WindowManager : public Service<WindowManager>
    {
    public:
        WindowManager() = default;
        ~WindowManager();

        /// Creates a new window and sets up a destruction callback.
        Window* CreateWindow(const char* title = nullptr, int w = 640, int h = 480, bool fullscrn = false, Uint32 flags = SDL_WINDOW_SHOWN);

        /// Handles window events. Returns nullptr unless a window receives SDL_WINDOWEVENT_CLOSE, in which case a pointer to that window is returned.
        Window* HandleEvent(SDL_Event& e);

    private:
        /// Called when a window is destroyed.
        void OnWindowDestroyed(Window& window);

        /// Window callback handles.
        map<Window*, int> windows;

    };
*/

}

#endif // WINDOW_H
