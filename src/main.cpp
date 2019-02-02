#include <cstdio>
#include <string.h>
#include <SDL.h>

#include "ossium/config.h"
#include "ossium/init.h"
#include "ossium/window.h"
#include "ossium/time.h"
#include "ossium/font.h"
#include "ossium/text.h"
#include "ossium/renderer.h"
#include "ossium/ecs.h"
#include "ossium/delta.h"
#include "ossium/keyboard.h"
#include "ossium/mouse.h"
#include "ossium/audio.h"

#ifdef UNIT_TESTS
#include "ossium/testmodules.h"
using namespace ossium::test;
#endif // UNIT_TESTS

using namespace std;
using namespace ossium;

Text* targetText = nullptr;
Text* mainText = nullptr;
int posx = 0;
int posy = 0;
bool check_for_key = false;
bool update_binding = false;
SDL_Keycode currentKey = SDLK_SPACE;

void MouseScrollAction(const MouseInput& data)
{
    posx -= data.x * 8;
    posy -= data.y * 8;
}

void MouseClickAction(const MouseInput& data)
{
    if (mainText != nullptr)
    {
        if (data.state == MOUSE_RELEASED)
        {
            mainText->setColor(colour::GREEN);
            mainText->setText("Press any key to bind it to the FPS text");
            check_for_key = true;
        }
    }
}

void GetKey(const KeyboardInput& data)
{
    if (check_for_key)
    {
        currentKey = data.key;
        mainText->setColor(colour::RED);
        mainText->setText("Current FPS key is " + (string)SDL_GetKeyName(currentKey));
        check_for_key = false;
        update_binding = true;
    }
}

void KeyAction(const KeyboardInput& data)
{
    if (targetText != nullptr)
    {
        if (data.state == KEY_DOWN)
        {
            targetText->setColor(colour::YELLOW);
            targetText->setBox(true);
            targetText->setBackgroundColor(colour::RED);
        }
        else
        {
            targetText->setColor(colour::CYAN);
            targetText->setBox(false);
        }
    }
}

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

        ///
        /// ECS and text rendering demo
        ///

        Font font;
        int ptsizes[3] = {2, 24, 36};
        font.load("Orkney Regular.ttf", &ptsizes[0]);
        font.init("Orkney Regular.ttf");

        Entity gameObject;
        gameObject.SetName("Test Entity");
        gameObject.AttachComponent<Text>();

        targetText = gameObject.GetComponent<Text>();
        if (targetText != nullptr)
        {
            targetText->setText("FPS: 0");
            targetText->setColor(colour::CYAN);
            targetText->textToTexture(mainRenderer, &font);
        }

        gameObject.AttachComponent<Text>();
        vector<Text*> compList = gameObject.GetComponents<Text>();
        if (!compList.empty() && compList.size() > 1)
        {
            compList[1]->setColor(colour::RED);
            compList[1]->setText("Current FPS key is " + (string)SDL_GetKeyName(currentKey));
            compList[1]->textToTexture(mainRenderer, &font, 36);
            mainText = compList[1];
        }

        ///
        /// Input handling demo
        ///

        InputContext mainContext;
        KeyboardHandler* keyboard = mainContext.AddHandler<KeyboardHandler>();

        keyboard->AddAction("yellow_text", *KeyAction);
        keyboard->Bind("yellow_text", SDLK_SPACE);
        keyboard->AddBindlessAction(*GetKey);

        MouseHandler* mouse = mainContext.AddHandler<MouseHandler>();

        mouse->AddAction("mouseclick", *MouseClickAction);
        mouse->AddAction("scroll", *MouseScrollAction);
        mouse->Bind("mouseclick", MOUSE_BUTTON_LEFT);
        mouse->Bind("scroll", MOUSE_WHEEL);

        Input input;

        input.AddContext("main", &mainContext);

        ///
        /// Audio demo
        ///

        AudioClip sound;
        if (!sound.load("test_audio.wav"))
        {
            SDL_Log("Error loading sound! Mix_Error: %s", Mix_GetError());
        }
        else
        {
            AudioSource source;
            source.Play(&sound, 1.0f, -1);
        }

        ///
        /// Init timing stuff before we start the main loop
        ///

        Timer fpsTimer;
        fpsTimer.start();
        int countedFrames = 0;

        /// Initialise the global delta time and FPS controller
        global::delta.init(settings);

        while (!quit)
        {
            /// Input handling phase
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.handle_events(e);
                if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
                {
                    quit = true;
                    break;
                }
                input.HandleEvent(e);
            }

            /// Demo dynamic key binding
            if (update_binding)
            {
                /// Rebind the key
                keyboard->Bind("yellow_text", currentKey);
                update_binding = false;
            }

            /// Logic update phase
            Entity::ecs_info.UpdateComponents();

            /// Rendering phase
            mainRenderer->renderClear();
            vector<Text*> handyComponents = gameObject.GetComponents<Text>();
            if (!handyComponents.empty())
            {
                for (int i = 0, counti = handyComponents.size(); i < counti; i++)
                {
                    handyComponents[i]->renderSimple(mainRenderer, posx, (i * 50) + posy);
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
            mainRenderer->enqueue(&viewrect, 0, false, colour::WHITE);
            mainRenderer->renderAll(-1);
            mainRenderer->renderPresent();

            /// Update timer and FPS count
            countedFrames++;
            global::delta.update();
        }
    }
    TerminateOssium();
    return 0;
}
