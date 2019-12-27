/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
        /// Appropriate constructor and destructor
        Window(const char* title = "Ossium Engine", int w = 640, int h = 480, bool fullscrn = false, Uint32 flags = SDL_WINDOW_SHOWN);
        ~Window();

        /// Handles window events
        int HandleEvent(SDL_Event &event);

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

}

#endif // WINDOW_H
