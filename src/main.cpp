#include <cstdio>
#include <string.h>
#include <SDL2/SDL.h>

#include "ossium/config.h"
#include "ossium/init.h"
#include "ossium/window.h"
#include "ossium/time.h"
#include "ossium/font.h"
#include "ossium/text.h"
#include "ossium/renderer.h"
#include "ossium/ecs.h"
#include "ossium/delta.h"

#ifdef UNIT_TESTS
#include "ossium/testmodules.h"
using namespace ossium::test;
#endif // UNIT_TESTS

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
        #ifdef UNIT_TESTS
        TEST_RUN(CircularBufferTests);
        TEST_RUN(BasicUtilsTests);
        TEST_RUN(TreeTests);
        TEST_RUN(FSM_Tests);
        TEST_RUN(EventSystemTests);
        TEST_RUN(CSV_Tests);
        TEST_EVALUATE();
        return 0;
        #endif // UNIT_TESTS

        /// Load configuration settings
        Config settings;
        LoadConfig(&settings);

        /// Create the window
        Window mainWindow("Ossium Engine", 1024, 768, settings.fullscreen, SDL_WINDOW_SHOWN);

        /// Create renderer
        Renderer* mainRenderer = new Renderer(&mainWindow, 5, true, settings.vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
        mainWindow.setAspectRatio(16, 9);

        SDL_Event e;

        Font font;
        int ptsizes[3] = {2, 24, 48};
        font.load("Orkney Regular.ttf", &ptsizes[0]);
        font.init("Orkney Regular.ttf");

        Entity gameObject;
        gameObject.SetName("Test Entity");
        gameObject.AttachComponent<Text>();

        Text* targetText = gameObject.GetComponent<Text>();
        if (targetText != nullptr)
        {
            targetText->setText("FPS: 0");
            targetText->setColor(CYAN);
            targetText->textToTexture(mainRenderer, &font);
        }

        gameObject.AttachComponent<Text>();
        vector<Text*> compList = gameObject.GetComponents<Text>();
        if (!compList.empty() && compList.size() > 1)
        {
            compList[1]->setColor(RED);
            compList[1]->setText("Text Component Test");
            compList[1]->textToTexture(mainRenderer, &font, 48);
        }

        Timer fpsTimer;
        fpsTimer.start();
        int countedFrames = 0;

        /// Initialise the global delta time and FPS controller
        global::delta.init(settings);

        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.handle_events(e);
                if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
                {
                    quit = true;
                    break;
                }
            }

            /// Update phase
            Entity::ecs_info.UpdateComponents();

            /// Rendering phase
            mainRenderer->renderClear();
            vector<Text*> handyComponents = gameObject.GetComponents<Text>();
            if (!handyComponents.empty())
            {
                for (int i = 0, counti = handyComponents.size(); i < counti; i++)
                {
                    handyComponents[i]->renderSimple(mainRenderer, 0, i * 50);
                }
                if (fpsTimer.getTicks() > 250)
                {
                    handyComponents[0]->setText("FPS: " + ToString((int)(countedFrames / (fpsTimer.getTicks() / 1000.0f))));
                    countedFrames = 0;
                    fpsTimer.start();
                }
            }
            SDL_SetRenderDrawColor(mainRenderer->getRenderer(), 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect viewrect = mainWindow.getViewportRect();
            viewrect.x = 0;
            viewrect.y = 0;
            mainRenderer->enqueue(&viewrect, 0, false, WHITE);
            mainRenderer->renderAll(-1);
            mainRenderer->renderPresent();

            countedFrames++;
            global::delta.update();
        }
    }
    TerminateOssium();
    return 0;
}
