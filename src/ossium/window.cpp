#include <SDL.h>

#include "basics.h"
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
        fixed_aspect = false;
        width = w;
        height = h;
        aspect_width = 0;
        aspect_height = 0;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
        if (window == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window creation failed! SDL_Error: %s", SDL_GetError());
            throw;
        }

        if (fullscreen)
        {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
        }

        /// Get max display size
        SDL_Rect display_bounds;
        int index = SDL_GetWindowDisplayIndex(window);
        if (index < 0 || SDL_GetDisplayBounds(index, &display_bounds) != 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get primary display bounds! Defaulting to initial window size. SDL_Error: %s", SDL_GetError());
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
        SDL_DestroyWindow(window);
        window = NULL;
    }

    void Window::handle_events(SDL_Event &event)
    {
        switch (event.window.event)
        {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
                width = event.window.data1;
                height = event.window.data2;
                updateViewport();
                break;
            }
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

    int Window::getAspectWidth()
    {
        return aspect_width;
    }

    int Window::getAspectHeight()
    {
        return aspect_height;
    }

    SDL_Rect Window::getViewportRect()
    {
        return viewportRect;
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
        updateViewport();
        fullscreen = true;
    }

    void Window::setWindowed()
    {
        SDL_SetWindowFullscreen(window, SDL_FALSE);
        updateViewport();
        fullscreen = false;
    }

    void Window::setBordered()
    {
        SDL_SetWindowBordered(window, SDL_TRUE);
        border = true;
    }

    void Window::setBorderless()
    {
        SDL_SetWindowBordered(window, SDL_FALSE);
        border = false;
    }

    void Window::setAspectRatio(int aspect_w, int aspect_h, bool fixed)
    {
        fixed_aspect = fixed;
        if (aspect_w < 1)
        {
            aspect_w = 1;
        }
        if (aspect_h < 1)
        {
            aspect_h = 1;
        }
        aspect_width = aspect_w;
        aspect_height = aspect_h;
        updateViewport();
    }

    void Window::updateViewport()
    {
        SDL_Renderer* renderer = SDL_GetRenderer(window);
        if (renderer == NULL)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Cannot update viewport; window has no associated renderer! SDL_Error: %s", SDL_GetError());
            return;
        }
        SDL_Rect viewRect;
        float percent_width = 1.0f;
        float percent_height = 1.0f;
        if (fullscreen)
        {
            percent_width = (float)display_width / (float)aspect_width;
            percent_height = (float)display_height / (float)aspect_height;
        }
        else
        {
            percent_width = (float)width / (float)aspect_width;
            percent_height = (float)height / (float)aspect_height;
        }
        /// Get the smallest percent and use that to scale dimensions
        float smallest_percent;
        if (percent_width < percent_height)
        {
            smallest_percent = percent_width;
        }
        else
        {
            smallest_percent = percent_height;
        }
        if (fixed_aspect)
        {
            smallest_percent = clamp(smallest_percent, 0.0f, 1.0f);
        }
        viewRect.h = (int)(smallest_percent * (!fullscreen ? (float)aspect_height : (float)display_height));
        viewRect.w = (int)(smallest_percent * (!fullscreen ? (float)aspect_width : (float)display_width));
        /// Calculate viewport anchor position
        int deltaw = (width - viewRect.w);
        int deltah = (height - viewRect.h);
        if (deltaw > 0)
        {
            viewRect.x = deltaw / 2;
        }
        else
        {
            viewRect.x = 0;
        }
        if (deltah > 0)
        {
            viewRect.y = deltah / 2;
        }
        else
        {
            viewRect.y = 0;
        }
        SDL_RenderSetViewport(renderer, &viewRect);
        viewportRect = viewRect;
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
