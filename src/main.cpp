#include <cstdio>
#include <SDL2/SDL.h>

#include "oss_window.h"
#include "oss_texture.h"
#include "oss_timer.h"

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
        OSS_Timer timer;
        float prevtime = 0.0;
        float deltatime = 0.0;
        OSS_Window windowTest("Test", 640, 480);
        SDL_Event e;
        OSS_Texture textureTest;
        SDL_Window* window_sdl = windowTest.getWindow();
        SDL_Renderer* renderer = SDL_CreateRenderer(window_sdl, -1, SDL_RENDERER_ACCELERATED);
        if (!textureTest.loadImage("test.png", renderer, SDL_GetWindowPixelFormat(window_sdl)))
        {
            SDL_Log("%s", SDL_GetError());
        }
        float angle = 0.0;
        timer.start();
        while (!quit)
        {
            deltatime = (timer.getTicks() - prevtime) / 1000.0;
            prevtime = timer.getTicks();
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
            angle += 90.0 * deltatime;
            if (angle >= 360.0)
            {
                angle -= 360.0;
            }
            textureTest.render(renderer, 0, 0, NULL, angle);
            SDL_RenderPresent(renderer);
        }
    }
    return 0;
}
