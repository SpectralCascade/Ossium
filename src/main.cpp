#include <cstdio>
#include <string.h>
#include <SDL2/SDL.h>

#include "oss_config.h"
#include "oss_init.h"
#include "oss_window.h"
#include "oss_texture.h"
#include "oss_time.h"
#include "oss_font.h"
#include "oss_text.h"
#include "oss_vector.h"
#include "oss_primitives.h"
#include "oss_statesprite.h"
#include "oss_resourcecontroller.h"

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
        OSS_ResourceController<OSS_Texture> textureController;
        if (textureController.loadResource("test.png"))
        {
            if (!textureController.postLoadInit("test.png", mainRenderer))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error during post-load initialisation of resource 'testing.png'!");
            }
        }

        /// Create font manager
        /*OSS_ResourceController<OSS_Font> fontController;
        fontController.loadResource("consolas.ttf");
        int fontSize = 56;
        fontController.loadResource("serif.ttf", &fontSize);
        /// Test fonts
        OSS_Text testOne;
        OSS_Text testTwo;
        testTwo.setStyling(TTF_STYLE_UNDERLINE | TTF_STYLE_BOLD | TTF_STYLE_ITALIC, 2, TTF_HINTING_NONE);
        testTwo.setRenderMode(OSS_RENDERTEXT_BLEND);
        testTwo.setColor({0xFF, 0x00, 0x00, 0xFF});
        testOne.setText("Muhahahaha! FONTS!");
        testOne.setBackgroundColor({0xFF, 0x00, 0x00, 0xFF});
        testOne.setRenderMode(OSS_RENDERTEXT_BLEND);
        testOne.textToTexture(mainRenderer, fontController.getResource("consolas.ttf")->getFont());
        testTwo.setText("Fancy font :D");
        testTwo.textToTexture(mainRenderer, fontController.getResource("serif.ttf")->getFont());
        testOne.setBox(true);*/

        /// State sprite and primitives for testing
        OSS_Rectangle buttonRect = {0, 0, 640, 240};
        OSS_StateSprite buttonSprite;
        buttonSprite.addState("idle", textureController.getResource("test.png"), true, 2);

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
                if (e.type == SDL_MOUSEMOTION)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    OSS_Vector vec = {(float)x, (float)y};
                    if (OSS_Intersect(vec, buttonRect))
                    {
                        buttonSprite.changeSubState(1);
                    }
                    else
                    {
                        buttonSprite.changeSubState(0);
                    }
                }
            }
            SDL_RenderClear(mainRenderer);
            if (textureController.getResource("test.png") != NULL)
            {
                buttonSprite.render(mainRenderer, buttonRect.x, buttonRect.y);
            }
            //testOne.renderSimple(mainRenderer, 0, 0, NULL);
            //testTwo.renderSimple(mainRenderer, 320 - (testTwo.getWidth() / 2), 240 - (testTwo.getHeight() / 2), NULL);
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderPresent(mainRenderer);
        }
    }
    return 0;
}
