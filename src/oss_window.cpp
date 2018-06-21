#include <SDL2/SDL.h>

#include "oss_window.h"

OSS_Window::OSS_Window(const char* title, int width, int height, bool fullscrn)
{
    window = NULL;
    minimized = false;
    fullscreen = fullscrn;
    focus = true;
    border = true;

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    if (fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_TRUE);
    }
}

OSS_Window::~OSS_Window()
{
    SDL_DestroyWindow(window);
    window = NULL;
}

void OSS_Window::handle_events(SDL_Event &event)
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

SDL_Window* OSS_Window::getWindow()
{
    return window;
}

void OSS_Window::setFullScreen()
{
    SDL_SetWindowFullscreen(window, SDL_TRUE);
    fullscreen = true;
}

void OSS_Window::setWindowed()
{
    SDL_SetWindowFullscreen(window, SDL_FALSE);
}

void OSS_Window::setBorder(SDL_bool bordered)
{
    SDL_SetWindowBordered(window, bordered);
}

bool OSS_Window::isMinimized()
{
    return minimized;
}

bool OSS_Window::isFullscreen()
{
    return fullscreen;
}

bool OSS_Window::isFocus()
{
    return focus;
}
