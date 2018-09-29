#ifndef OSS_INIT_H
#define OSS_INIT_H

#include <SDL2/SDL.h>

/// Initialisation error types
enum InitError
{
    OSS_INIT_ERROR_SDL = -9999,
    OSS_INIT_ERROR_WINDOW,
    OSS_INIT_ERROR_RENDERER,
    OSS_INIT_ERROR_IMG,
    OSS_INIT_ERROR_MIXER,
    OSS_INIT_ERROR_TTF
};

/// Initialises SDL and other engine subsystems; returns InitError value. No error if >= 0
int OSS_Init();

/// Creates and returns a renderer
SDL_Renderer* OSS_CreateRenderer(SDL_Window* window, bool vsync = false);

#endif // OSS_INIT_H
