#include <cstdio>
#include <SDL2/SDL.h>

#include "oss_window.h"
#include "oss_texture.h"

using namespace std;

int main(int argc, char* argv[])
{
    bool quit = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error initialising SDL 2.");
    }
    else
    {
        OSS_Window windowTest("Test", 640, 480);
        SDL_Event e;
        OSS_Texture textureTest;
        SDL_Window* window_sdl = windowTest.getWindow();
        SDL_Renderer* renderer = SDL_CreateRenderer(window_sdl, -1, SDL_RENDERER_ACCELERATED);
        if (!textureTest.loadImage("test.png", renderer, SDL_GetWindowPixelFormat(window_sdl)))
        {
            SDL_Log("%s", SDL_GetError());
        }
        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                windowTest.handle_events(e);
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                    break;
                }
            }
            SDL_RenderClear(renderer);
            textureTest.render(renderer, 0, 0);
            SDL_RenderPresent(renderer);
        }
    }
    return 0;
}
