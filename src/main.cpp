#include <cstdio>
#include <string.h>
#include <SDL2/SDL.h>

#include "oss_config.h"
#include "oss_init.h"
#include "oss_window.h"
#include "oss_texture.h"
#include "oss_timer.h"
#include "oss_resourcemanager.h"

using namespace std;

int main(int argc, char* argv[])
{
    bool quit = false;
    if (OSS_Init() < 0)
    {
        printf("ERROR: Failed to initialise Ossium engine.\n");
    }
    else
    {
        /// Load configuration settings
        OSS_Config settings;
        OSS_LoadConfig(&settings);

        /// Create the window
        OSS_Window mainWindow("Ossium Engine", 640, 480, settings.fullscreen);

        /// Create renderer
        SDL_Renderer* mainRenderer = OSS_CreateRenderer(mainWindow.getWindow(), settings.vsync);

        /// Create texture manager
        OSS_ResourceManager<OSS_Texture> textureManager;
        textureManager.loadTexture("testing.png", mainRenderer, SDL_GetWindowPixelFormat(mainWindow.getWindow()));

        /// Change pixel filtering setting
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, &settings.filtering);

        OSS_Timer timer;
        float prevtime = 0.0;
        float deltatime = 0.0;
        SDL_Event e;

        float angle = 0.0;
        timer.start();
        while (!quit)
        {
            deltatime = (timer.getTicks() - prevtime) / 1000.0;
            prevtime = timer.getTicks();
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.handle_events(e);
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                    break;
                }
            }
            SDL_RenderClear(mainRenderer);
            angle += 25.0 * deltatime;
            if (angle >= 360.0)
            {
                angle -= 360.0;
            }
            if (textureManager.getResource("test.png") != NULL)
            {
                textureManager.getResource("test.png")->render(mainRenderer, 0, 0, NULL, angle);
            }
            SDL_RenderPresent(mainRenderer);
        }
    }
    return 0;
}
