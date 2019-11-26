#include "funcutils.h"
#include "window.h"

namespace Ossium
{

    Window::Window(const char* title, int w, int h, bool fullscrn, Uint32 flags)
    {
        window = NULL;
        minimized = false;
        fullscreen = fullscrn;
        focus = true;
        border = true;
        width = w;
        height = h;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
        if (window == NULL)
        {
            Logger::EngineLog().Error("Window creation failed! SDL_Error: {0}", SDL_GetError());
            throw;
        }

        if (fullscreen)
        {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
            OnFullscreen(*this);
        }

        /// Get max display size
        SDL_Rect display_bounds;
        int index = SDL_GetWindowDisplayIndex(window);
        if (index < 0 || SDL_GetDisplayBounds(index, &display_bounds) != 0)
        {
            Logger::EngineLog().Error("Failed to get primary display bounds! Defaulting to initial window size. SDL_Error: {0}", SDL_GetError());
            display_width = width;
            display_height = height;
        }
        else
        {
            display_width = display_bounds.w;
            display_height = display_bounds.h;
        }

    }

    Window::~Window()
    {
        OnDestroyed(*this);
        SDL_DestroyWindow(window);
        window = NULL;
    }

    int Window::HandleEvent(SDL_Event &event)
    {
        if (event.type != SDL_WINDOWEVENT || event.window.windowID != SDL_GetWindowID(window))
        {
            /// If not a window event or the id doesn't match this window, early out.
            return 0;
        }
        switch (event.window.event)
        {
            case SDL_WINDOWEVENT_CLOSE:
            {
                return -1;
            }
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
                width = event.window.data1;
                height = event.window.data2;
                OnSizeChanged(*this);
                break;
            }
            case SDL_WINDOWEVENT_MINIMIZED:
            {
                minimized = true;
                OnMinimise(*this);
                break;
            }
            case SDL_WINDOWEVENT_MAXIMIZED:
            {
                minimized = false;
                OnMaximise(*this);
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
                focus = true;
                OnFocusGained(*this);
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST:
            {
                focus = false;
                OnFocusLost(*this);
                break;
            }
        }
        return 1;
    }

    SDL_Window* Window::GetWindow()
    {
        return window;
    }

    int Window::GetWidth()
    {
        SDL_GetWindowSize(window, &width, &height);
        return width;
    }

    int Window::GetHeight()
    {
        SDL_GetWindowSize(window, &width, &height);
        return height;
    }

    int Window::GetDisplayWidth()
    {
        return display_width;
    }

    int Window::GetDisplayHeight()
    {
        return display_height;
    }

    void Window::SetWidth(int newWidth)
    {
        width = newWidth;
        SDL_SetWindowSize(window, width, height);
        OnSizeChanged(*this);
    }

    void Window::SetHeight(int newHeight)
    {
        height = newHeight;
        SDL_SetWindowSize(window, width, height);
        OnSizeChanged(*this);
    }

    void Window::SetFullscreen()
    {
        SDL_SetWindowFullscreen(window, SDL_TRUE);
        OnFullscreen(*this);
        fullscreen = true;
    }

    void Window::SetWindowed()
    {
        SDL_SetWindowFullscreen(window, SDL_FALSE);
        OnWindowed(*this);
        fullscreen = false;
    }

    void Window::SetBordered()
    {
        SDL_SetWindowBordered(window, SDL_TRUE);
        border = true;
    }

    void Window::SetBorderless()
    {
        SDL_SetWindowBordered(window, SDL_FALSE);
        border = false;
    }

    bool Window::IsMinimised()
    {
        return minimized;
    }

    bool Window::IsFullscreen()
    {
        return fullscreen;
    }

    bool Window::IsFocus()
    {
        return focus;
    }

    ///
    /// WindowManager
    ///

    WindowManager::~WindowManager()
    {
        /// Remove all callbacks
        for (auto i : windows)
        {
            /// Prevent invalid callbacks.
            i.first->OnDestroyed -= i.second;
        }
    }

    /// Creates a new window
    Window* WindowManager::CreateWindow(const char* title, int w, int h, bool fullscrn, Uint32 flags)
    {
        Window* window = new Window(title, w, h, fullscrn, flags);
        windows[window] = window->OnDestroyed += [this] (Window& w) { OnWindowDestroyed(w); };
        return window;
    }

    Window* WindowManager::HandleEvent(SDL_Event& event)
    {
        for (auto window : windows)
        {
            if (window.first->HandleEvent(event) < 0)
            {
                return window.first;
            }
        }
        return nullptr;
    }

    void WindowManager::OnWindowDestroyed(Window& window)
    {
        /// Remove the window reference and callback handle.
        auto itr = windows.find(&window);
        if (itr != windows.end())
        {
            windows.erase(itr);
        }
    }

}
