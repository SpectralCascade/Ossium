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
        if (textureManager.loadResource("test.png"))
        {
            if (!textureManager.postLoadInit("test.png", mainRenderer))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error during post-load initialisation of resource 'testing.png'!");
            }
        }

        /// Change pixel filtering setting ("0" = no filter, "1" = linear, "2" = bilinear [directX/direct3D only])
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, &settings.filtering);

        OSS_Timer timer;
        float prevtime = 0.0;
        float deltatime = 0.0;
        SDL_Event e;

        float zoom = 1.0;
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
            zoom += 0.1 * deltatime;
            if (zoom >= 1.0)
            {
                zoom = 0.1;
            }
            if (textureManager.getResource("test.png") != NULL)
            {
                float width = textureManager.getResource("test.png")->getWidth() * zoom;
                float height = textureManager.getResource("test.png")->getHeight() * zoom;
                textureManager.getResource("test.png")->render(mainRenderer, { (int)(320 - (width / 2.0)), (int)(240 - (height / 2.0)), (int)width, (int)height}, NULL, zoom * 360.0);
            }
            SDL_RenderPresent(mainRenderer);
        }
    }
    return 0;
}
