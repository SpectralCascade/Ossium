#include <SDL2/SDL.h>

#include "window.h"

namespace ossium
    {
    Window::Window(const char* title, int width, int height, bool fullscrn)
    {
        window = NULL;
        minimized = false;
        fullscreen = fullscrn;
        focus = true;
        border = true;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window creation failed! SDL_Error: %s", SDL_GetError());
            throw;
        }

        if (fullscreen)
        {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
        }
    }

    Window::~Window()
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    void Window::handle_events(SDL_Event &event)
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT_MINIMIZED:
            {
                minimized = true;
                break;
            }
            case SDL_WINDOWEVENT_MAXIMIZED:
            {
                minimized = false;
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
                focus = true;
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST:
            {
                focus = false;
                break;
            }
        }
    }

    SDL_Window* Window::getWindow()
    {
        return window;
    }

    int Window::getWidth()
    {
        SDL_GetWindowSize(window, &width, &height);
        return width;
    }

    int Window::getHeight()
    {
        SDL_GetWindowSize(window, &width, &height);
        return height;
    }

    void Window::setWidth(int newWidth)
    {
        width = newWidth;
        SDL_SetWindowSize(window, width, height);
    }

    void Window::setHeight(int newHeight)
    {
        height = newHeight;
        SDL_SetWindowSize(window, width, height);
    }

    void Window::setFullScreen()
    {
        SDL_SetWindowFullscreen(window, SDL_TRUE);
        fullscreen = true;
    }

    void Window::setWindowed()
    {
        SDL_SetWindowFullscreen(window, SDL_FALSE);
        fullscreen = false;
    }

    void Window::setBorder(SDL_bool bordered)
    {
        SDL_SetWindowBordered(window, bordered);
        if (bordered == SDL_TRUE)
        {
            border = true;
        }
        else
        {
            border = false;
        }
    }

    bool Window::isMinimized()
    {
        return minimized;
    }

    bool Window::isFullscreen()
    {
        return fullscreen;
    }

    bool Window::isFocus()
    {
        return focus;
    }

}
