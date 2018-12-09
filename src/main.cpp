#include <cstdio>
#include <string.h>
#include <SDL2/SDL.h>

#include "ossium/config.h"
#include "ossium/init.h"
#include "ossium/window.h"
#include "ossium/texture.h"
#include "ossium/texturepack.h"
#include "ossium/time.h"
#include "ossium/font.h"
#include "ossium/text.h"
#include "ossium/vector.h"
#include "ossium/transform.h"
#include "ossium/primitives.h"
#include "ossium/statesprite.h"
#include "ossium/resourcecontroller.h"
#include "ossium/renderer.h"
#include "ossium/ecs.h"

using namespace std;
using namespace ossium;

int main(int argc, char* argv[])
{
    bool quit = false;
    if (InitialiseOssium() < 0)
    {
        printf("ERROR: Failed to initialise Ossium engine.\n");
    }
    else
    {
        /// Load configuration settings
        Config settings;
        LoadConfig(&settings);

        /// Create the window
        Window mainWindow("Ossium Engine", 640, 480, settings.fullscreen);

        /// Create renderer
        Renderer* mainRenderer = new Renderer(&mainWindow, 5, true, settings.vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);

        /// Create texture manager
        ResourceController<Texture> textures;
        /*if (textures.load("test.png"))
        {
            if (!textures.initialise("test.png", mainRenderer))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error during post-load initialisation of resource 'test.png'!");
            }
        }
        if (textures.load("sprite_test.png"))
        {
            if (!textures.initialise("sprite_test.png", mainRenderer))
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error during post-load initialisation of resource 'sprite_test.png'!");
            }
        }

        /// Create font manager
        ResourceController<Font> fonts;
        /// Load font with a default selection of point sizes
        int* fontPointSizes = new int[1];
        fontPointSizes[0] = 0;
        int* serifSize = new int[2];
        serifSize[0] = 1;
        serifSize[1] = 56;
        fonts.load("serif.ttf", serifSize);
        fonts.load("consolas.ttf", fontPointSizes);
        /// Test fonts
        Text testOne;
        Text testTwo;
        testTwo.setStyling(TTF_STYLE_UNDERLINE | TTF_STYLE_BOLD | TTF_STYLE_ITALIC, 2, TTF_HINTING_NONE);
        testTwo.setRenderMode(RENDERTEXT_BLEND);
        testTwo.setColor({0xFF, 0x00, 0x00, 0xFF});
        testTwo.setBackgroundColor({50, 0, 255, 255});
        testOne.setText("Muhahahaha! FONTS!");
        testOne.setBackgroundColor({0x00, 0x00, 0x00, 110});
        testOne.setBoxPaddingWidth(20);
        fonts.find("consolas.ttf");
        testOne.setRenderMode(RENDERTEXT_BLEND);
        testOne.textToTexture(mainRenderer, fonts.find("consolas.ttf"));
        testTwo.setText("Fancy font :D");
        testTwo.textToTexture(mainRenderer, fonts.find("serif.ttf"));
        testOne.setBox(true);

        /// State sprite and primitives for testing
        SDL_Rect buttonRect = {0, 0, 640, 240};
        StateSprite buttonSprite;
        buttonSprite.addState("idle", textures.find("test.png"), true, 2);

        SDL_Rect chestRect = {320 - 64, 300, 128, 100};
        StateSprite chestSprite;
        chestSprite.addState("open", textures.find("sprite_test.png"), false, 4);

        /// Change pixel filtering setting ("0" = no filter, "1" = linear, "2" = bilinear [directX/direct3D only])
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, &settings.filtering);

        Timer timer;
        float prevtime = 0.0;
        float deltatime = 0.0;*/
        SDL_Event e;

        /*
        TexturePack spriteAtlas;
        Uint32 winPixelFormat = SDL_GetWindowPixelFormat(mainWindow.getWindow());

        spriteAtlas.import("sprite_test.png", mainRenderer, winPixelFormat);
        spriteAtlas.import("pack_texture.png", mainRenderer, winPixelFormat);
        //spriteAtlas.import("noice.png", mainRenderer, SDL_GetWindowPixelFormat(mainWindow.getWindow()));

        spriteAtlas.packImported(mainRenderer, winPixelFormat);

        if (!spriteAtlas.save(mainRenderer, winPixelFormat, "test_pack"))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to save TexturePack at path '%s'!", "test_pack");
        }
        spriteAtlas.freeAll();

        // Test loading
        spriteAtlas.load("test_pack", NULL);
        spriteAtlas.init(mainRenderer);
        */

        Font font;
        int ptsizes[2] = {1, 24};
        font.load("serif.ttf", &ptsizes[0]);
        font.init("serif.ttf");

        Entity gameObject;
        gameObject.SetName("Test Entity");
        gameObject.AttachComponent<Text>();

        Text* targetText = gameObject.GetComponent<Text>();
        if (targetText != nullptr)
        {
            targetText->setText("Testing Text Component");
            targetText->setColor(CYAN);
            targetText->textToTexture(mainRenderer, &font);
        }

        gameObject.AttachComponent<Text>();
        vector<Text*> compList = gameObject.GetComponents<Text>();
        if (!compList.empty() && compList.size() > 1)
        {
            compList[1]->setColor(RED);
            compList[1]->setText("Another text component...");
            compList[1]->textToTexture(mainRenderer, &font);
        }

/*        SDL_SetRenderDrawBlendMode(mainRenderer->getRenderer(), SDL_BLENDMODE_BLEND);

        bool openChest = false;

        timer.start();*/
        while (!quit)
        {
/*            deltatime = (timer.getTicks() - prevtime) / 1000.0;
            prevtime = timer.getTicks();*/
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.handle_events(e);
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                    break;
                }
/*                if (e.type == SDL_MOUSEMOTION)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    SDL_Point pnt = {x, y};
                    if (IntersectSDL(pnt, buttonRect))
                    {
                        buttonSprite.changeSubState(1);
                    }
                    else
                    {
                        buttonSprite.changeSubState(0);
                    }
                    if (IntersectSDL(pnt, chestRect))
                    {
                        openChest = true;
                    }
                    else
                    {
                        openChest = false;
                        chestSprite.changeSubState(0);
                        timer.start();
                    }
                }*/
            }
/*            if (openChest)
            {
                chestSprite.changeSubState((int)((float)timer.getTicks() / 200.0f) % 4);
            }*/
            mainRenderer->renderClear();
/*            if (textures.find("test.png") != NULL)
            {
                buttonSprite.renderSimple(mainRenderer, (int)buttonRect.x, (int)buttonRect.y);
            }
            if (textures.find("sprite_test.png") != NULL)
            {
                chestSprite.renderSimple(mainRenderer, chestRect);
            }
            if (fonts.find("serif.ttf") != NULL && fonts.find("consolas.ttf") != NULL)
            {
                testOne.renderSimple(mainRenderer, 0, 0, NULL);
                testTwo.renderSimple(mainRenderer, 320 - (testTwo.getWidth() / 2), (480 / 2) - (testTwo.getHeight() / 2), NULL, 0);
            }*/
            /*SDL_Rect clip = spriteAtlas.getClip("sprite_test.png");
            spriteAtlas.getPackedTexture().renderSimple(mainRenderer, 0, 0, &clip);
            clip = spriteAtlas.getClip("pack_texture.png");
            spriteAtlas.getPackedTexture().renderSimple(mainRenderer, 256, 0, &clip);*/
            vector<Text*> handyComponents = gameObject.GetComponents<Text>();
            if (!handyComponents.empty())
            {
                for (int i = 0, counti = handyComponents.size(); i < counti; i++)
                {
                    handyComponents[i]->renderSimple(mainRenderer, 0, i * 50);
                }
            }
            SDL_SetRenderDrawColor(mainRenderer->getRenderer(), 0x00, 0x00, 0x00, 0xFF);
            mainRenderer->renderAll(-1);
            mainRenderer->renderPresent();
        }
    }
    TerminateOssium();
    return 0;
}
