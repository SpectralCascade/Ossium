#include <iostream>
#include <SDL2/SDL.h>

#include "oss_window.h"

using namespace std;

int main(int argc, char* argv[])
{
    bool quit = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Error initialising SDL 2." << endl;
    }
    else
    {
        OSS_Window windowTest("Test", 640, 480);
        SDL_Event e;
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
        }
    }
    return 0;
}
