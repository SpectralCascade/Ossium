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
        SDL_Event e;

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

        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.handle_events(e);
                if (e.type == SDL_QUIT)
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
            }
            SDL_SetRenderDrawColor(mainRenderer->getRenderer(), 0x00, 0x00, 0x00, 0xFF);
            mainRenderer->renderAll(-1);
            mainRenderer->renderPresent();
        }
    }
    TerminateOssium();
    return 0;
}
