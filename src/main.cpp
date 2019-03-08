#include <cstdio>
#include <string.h>
#include <SDL.h>

#include "Ossium/config.h"
#include "Ossium/init.h"
#include "Ossium/window.h"
#include "Ossium/time.h"
#include "Ossium/font.h"
#include "Ossium/text.h"
#include "Ossium/renderer.h"
#include "Ossium/ecs.h"
#include "Ossium/delta.h"
#include "Ossium/keyboard.h"
#include "Ossium/mouse.h"
#include "Ossium/audio.h"
#include "Ossium/colours.h"
#include "Ossium/statesprite.h"
#include "Ossium/sprite.h"
#include "teststuff.h"

#ifdef UNIT_TESTS
#include "Ossium/testmodules.h"
using namespace Ossium::test;
#endif // UNIT_TESTS

using namespace std;
using namespace Ossium;

Text* targetText = nullptr;
Text* mainText = nullptr;
int posx = 0;
int posy = 0;
bool check_for_key = false;
bool update_binding = false;
SDL_Keycode currentKey = SDLK_m;
float volume = 1.0f;
bool volume_change = false;
Sint16 panning = 0;
AudioBus master;

void MouseScrollAction(const MouseInput& data)
{
    posx -= data.x * 8;
    posy -= data.y * 8;
    volume += data.y * 0.02f;
    volume_change = true;
    panning -= data.y * 6;
}

void MouseClickAction(const MouseInput& data)
{
    if (mainText != nullptr)
    {
        if (data.state == MOUSE_RELEASED)
        {
            mainText->setColor(colours::GREEN);
            mainText->setText("Press any key to bind to action TOGGLE AUDIO");
            check_for_key = true;
        }
    }
}

void GetKey(const KeyboardInput& data)
{
    if (check_for_key && data.state == KEY_UP)
    {
        currentKey = data.key;
        mainText->setColor(colours::RED);
        mainText->setText("Current master mute key is " + (string)SDL_GetKeyName(currentKey));
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
            targetText->setBackgroundColor(colours::YELLOW);
        }
        else if (data.state == KEY_UP)
        {
            if (!master.IsMuted())
            {
                targetText->setBackgroundColor(colours::RED);
                master.Mute();
            }
            else
            {
                master.Unmute();
                targetText->setBackgroundColor(colours::GREEN);
            }
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
        TEST_RUN(ClockTests);
        TEST_EVALUATE();
        return 0;
        #endif // UNIT_TESTS

        /// Load configuration settings
        Config settings;
        LoadConfig(&settings);

        /// Create the window
        Window mainWindow("Ossium Engine", 1024, 768, settings.fullscreen, SDL_WINDOW_SHOWN);

        /// Create renderer
        Renderer mainRenderer(&mainWindow, 5, settings.vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
        mainWindow.setAspectRatio(16, 9);

        /// Create an EntityComponentSystem
        EntityComponentSystem entitySystem;

        ///
        /// ECS and text rendering demo
        ///

        Font font;
        int ptsizes[3] = {2, 24, 36};
        font.load("Orkney Regular.ttf", &ptsizes[0]);
        font.init("Orkney Regular.ttf");

        Entity gameObject(&entitySystem);
        gameObject.SetName("Test Entity");
        gameObject.AddComponent<Text>();

        targetText = gameObject.GetComponent<Text>();
        if (targetText != nullptr)
        {
            targetText->setText("FPS: 0");
            targetText->setColor(colours::BLUE);
            //targetText->setBox(true);
            targetText->setOutline(1);
            targetText->setBackgroundColor(colours::GREEN);
            targetText->textToTexture(mainRenderer, &font);
            mainRenderer.Register(targetText);
        }

        gameObject.AddComponent<Text>();
        vector<Text*> compList = gameObject.GetComponents<Text>();
        if (!compList.empty() && compList.size() > 1)
        {
            compList[1]->setColor(colours::RED);
            compList[1]->setText("Current master mute key is " + (string)SDL_GetKeyName(currentKey));
            compList[1]->textToTexture(mainRenderer, &font, 36);
            mainText = compList[1];
            mainRenderer.Register(compList[1]);
        }

        ///
        /// ECS + Sprite animation demo
        ///

        AnimatorTimeline timeline;

        timeline.AddTweeningFuncs((vector<CurveFunction>){tweening::Power2, tweening::Power3, tweening::Power4, tweening::Power5, tweening::Sine, tweening::SineHalf, tweening::SineQuarter,
                                  tweening::Cosine, tweening::CosineHalf, tweening::CosineQuarter, tweening::Overshoot});

        SpriteAnimation spriteAnim;
        /// We also cache the image as we want to revert the texture each frame, which is costly but allows fancy real time effects...
        spriteAnim.LoadAndInit("sprite_test.osa", mainRenderer, SDL_GetWindowPixelFormat(mainWindow.getWindow()), true);

        Entity other(&entitySystem);
        other.AddComponent<Sprite>();

        Sprite* sprite = other.GetComponent<Sprite>();
        if (sprite != nullptr)
        {
            //sprite->PlayAnimation(timeline, &spriteAnim, 0, -1, false);
            sprite->position.x = (float)(mainRenderer.GetWidth() / 2);
            sprite->position.y = (float)(mainRenderer.GetHeight() / 2);
            mainRenderer.Register(sprite);
            /// Grayscale effect
            /*spriteAnim.ApplyEffect([] (SDL_Color c, SDL_Point p) {
                Uint8 grayscale = (Uint8)(((float)c.r * 0.3f) + ((float)c.g * 0.59f) + ((float)c.b * 0.11f));
                c = Colour(grayscale, grayscale, grayscale, c.a);
                return c;
            });*/
        }

        ///
        /// Input handling demo
        ///

        InputContext mainContext;
        KeyboardHandler& keyboard = *mainContext.AddHandler<KeyboardHandler>();

        keyboard.AddAction("TOGGLE MUTE", *KeyAction);
        keyboard.Bind("TOGGLE MUTE", SDLK_m);
        keyboard.AddBindlessAction(*GetKey);

        MouseHandler& mouse = *mainContext.AddHandler<MouseHandler>();

        mouse.AddAction("mouseclick", *MouseClickAction);
        mouse.AddAction("scroll", *MouseScrollAction);
        mouse.Bind("mouseclick", MOUSE_BUTTON_LEFT);
        mouse.Bind("scroll", MOUSE_WHEEL);

        InputController mainInput;

        mainInput.AddContext("main", &mainContext);
        mainInput.AddContext("stickman", other.AddComponent<StickFighter>(&mainRenderer)->context);

        SDL_Event e;

        ///
        /// Audio demo
        ///

        AudioClip sound;
        AudioSource source;
        AudioBus sfx;

        SoundStream.Link(&master);
        SoundStream.Play("test_stream.wav", 0.5f, -1);

        master.Mute();
        /// Source goes into sfx bus
        source.Link(&sfx);
        /// The sfx bus goes into the master bus
        sfx.Link(&master);
        sfx.SetVolume(0);
        if (!sound.load("test_audio.wav"))
        {
            SDL_Log("Error loading sound! Mix_Error: %s", Mix_GetError());
        }
        else
        {
            source.Play(&sound, 0.1f, -1);
        }

        ///
        /// Init timing stuff before we start the main loop
        ///

        Timer fpsTimer;
        fpsTimer.Start();
        int countedFrames = 0;

        /// Initialise the global delta time and FPS controller
        global::delta.init(settings);

        Point lightSpot(0.0f, 0.0f);

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
                mainInput.HandleEvent(e);
            }

            SDL_Rect viewrect = mainWindow.getViewportRect();

            int mx, my;
            SDL_GetMouseState(&mx, &my);
            mx -= viewrect.x;
            my -= viewrect.y;

            lightSpot = sprite->ScreenToLocalPoint(Point((float)mx, (float)my));

            timeline.Update(global::delta.time());

            /// Logic update phase
            if (volume_change)
            {
                source.SetPanning(panning);
                volume_change = false;
            }

            sfx.FadeIn(global::delta.time(), 3.0f);
            /// Demo dynamic key binding
            if (update_binding)
            {
                /// Rebind the key
                keyboard.Bind("TOGGLE MUTE", currentKey);
                update_binding = false;
            }

            //SDL_Log("dtime is %f before update", global::delta.time());
            entitySystem.UpdateComponents();
            //SDL_Log("dtime is %f after update", global::delta.time());
            /// Note: for some reason this bit crashes the engine after a brief time. Something to do with the animator.
            //spriteAnim.Init(mainRenderer, SDL_GetWindowPixelFormat(mainWindow.getWindow()), true);

            /// Pixel-precise lighting effect that stays constant despite sprite size changes
            //SDL_Rect area = sprite->GetClip();
            /*spriteAnim.ApplyEffect([&lightSpot, &sprite] (SDL_Color c, SDL_Point p) {
                float brightness = 1.0f - mapRange(clamp(lightSpot.DistanceSquared((Point){(float)p.x, (float)p.y}), 0.0f, (96.0f / sprite->GetRenderWidth()) * (96.0f / sprite->GetRenderWidth())), 0.0f, (100.0f / sprite->GetRenderWidth()) * (100.0f / sprite->GetRenderWidth()), 0.0f, 1.0f);
                c = Colour((Uint8)(brightness * (float)c.r), (Uint8)(brightness * (float)c.g), (Uint8)(brightness * (float)c.b), c.a);
                return c;
            });*/

            /// Rendering phase
            SDL_RenderClear(mainRenderer.GetRendererSDL());

            vector<Text*> handyComponents = gameObject.GetComponents<Text>();
            if (!handyComponents.empty())
            {
                for (int i = 0, counti = handyComponents.size(); i < counti; i++)
                {
                    handyComponents[i]->position.x = (handyComponents[i]->getWidth() / 2) + posx;
                    handyComponents[i]->position.y = (handyComponents[i]->getHeight() / 2) + (i * 50) + posy;
                }
                if (fpsTimer.GetTicks() > 250)
                {
                    handyComponents[0]->setText("FPS: " + ToString((int)(countedFrames / (fpsTimer.GetTicks() / 1000.0f))));
                    countedFrames = 0;
                    fpsTimer.Start();
                }
            }
            mainRenderer.RenderPresent(true);

            mainRenderer.SetDrawColour(colours::RED);
            viewrect.x = 0;
            viewrect.y = 0;
            SDL_RenderDrawRect(mainRenderer.GetRendererSDL(), &viewrect);

            mainRenderer.SetDrawColour(colours::BLACK);
            SDL_RenderPresent(mainRenderer.GetRendererSDL());

            /// Update timer and FPS count
            countedFrames++;
            global::delta.update();
        }
    }
    TerminateOssium();
    return 0;
}
