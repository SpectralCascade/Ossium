#include <cstdio>
#include <string.h>
#include <SDL.h>
#include <Box2D/Box2D.h>

#include "Ossium.h"
#include "teststuff.h"

#ifdef UNIT_TESTS
#include "Ossium/testmodules.h"
using namespace Ossium::Test;
#endif // UNIT_TESTS

using namespace std;
using namespace Ossium;

Text* tarGetText = nullptr;
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

ActionOutcome MouseScrollActionOutcome(const MouseInput& data)
{
    posx -= data.x * 8;
    posy -= data.y * 8;
    volume += data.y * 0.02f;
    volume_change = true;
    panning -= data.y * 6;
    return ActionOutcome::ClaimContext;
}

ActionOutcome MouseClickActionOutcome(const MouseInput& data)
{
    if (mainText != nullptr)
    {
        if (data.state == MOUSE_RELEASED)
        {
            mainText->SetColor(Colors::GREEN);
            mainText->SetText("Press any key to bind to action TOGGLE AUDIO");
            check_for_key = true;
            return ActionOutcome::ClaimContext;
        }
    }
    return ActionOutcome::Ignore;
}

ActionOutcome GetKey(const KeyboardInput& data)
{
    if (check_for_key)
    {
        if (data.state == KEY_UP)
        {
            currentKey = data.key;
            mainText->SetColor(Colors::RED);
            mainText->SetText("Current master mute key is " + (string)SDL_GetKeyName(currentKey));
            check_for_key = false;
            update_binding = true;
        }
        return ActionOutcome::ClaimGlobal;
    }
    return ActionOutcome::Ignore;
}

ActionOutcome KeyAction(const KeyboardInput& data)
{
    if (tarGetText != nullptr)
    {
        if (data.state == KEY_DOWN)
        {
            tarGetText->SetBackgroundColor(Colors::YELLOW);
        }
        else if (data.state == KEY_UP)
        {
            if (!master.IsMuted())
            {
                tarGetText->SetBackgroundColor(Colors::RED);
                master.Mute();
            }
            else
            {
                master.Unmute();
                tarGetText->SetBackgroundColor(Colors::GREEN);
            }
        }
        return ActionOutcome::ClaimContext;
    }
    return ActionOutcome::Ignore;
}

int main(int argc, char* argv[])
{

    /// PHYSICS TESTING

    B2_NOT_USED(argc);
	B2_NOT_USED(argv);

	// Define the gravity vector (upside down for now as SDL inverts the Y axis).
	b2Vec2 gravity(0.0f, 9.81f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(PTM(1024 / 2), PTM(478.0f));

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(PTM(1024 / 2), PTM(1));

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(PTM(50), PTM(4.0f));
	bodyDef.angularVelocity = -4;
	bodyDef.linearVelocity = b2Vec2(PTM(50), 0);
	b2Body* body = world.CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(PTM(32), PTM(32));

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.2f;

	/// Bounciness!
	fixtureDef.restitution = 0.6f;

	// Add the shape to the body.
	body->CreateFixture(&fixtureDef);

	int velocityIterations = 6;
	int positionIterations = 2;

	/// END PHYSICS SETUP

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
        //TEST_RUN(CSV_Tests);
        TEST_RUN(JSON_Tests);
        TEST_RUN(ClockTests);
        TEST_RUN(SchemaTests);
        TEST_RUN(RandTests);
        TEST_RUN(EntitySerialisationTests);
        TEST_EVALUATE();
        /*cout << "Enter any text to quit." << endl;
        string a;
        cin >> a;*/
        return 0;
        #endif // UNIT_TESTS

        /// Load configuration settings
        Config settings;
        LoadConfig(&settings);

        /// Create the window
        Window mainWindow("Ossium Engine", 1024, 768, settings.fullscreen, SDL_WINDOW_SHOWN);

        /// Create renderer
        Renderer mainRenderer(&mainWindow, 5, settings.vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
        //mainRenderer.SetAspectRatio(16, 9);

        /// Create an EntityComponentSystem
        EntityComponentSystem entitySystem;

        /*b2World physWorld;
        physWorld.*/

        ///
        /// ECS and text rendering demo
        ///

        Font font;
        int ptsizes[3] = {2, 24, 36};
        font.Load("assets/Orkney Regular.ttf", &ptsizes[0]);

        Entity& debugDrawEntity = *entitySystem.CreateEntity();
        debugDrawEntity.SetName("Debug Draw");
        DebugDraw& debugDraw = *debugDrawEntity.AddComponent<DebugDraw>(&mainRenderer, mainRenderer.GetNumLayers() - 1);

        Entity& gameObject = *entitySystem.CreateEntity();
        gameObject.SetName("Test Entity");
        gameObject.AddComponent<Text>(&mainRenderer, 2);

        tarGetText = gameObject.GetComponent<Text>();
        if (tarGetText != nullptr)
        {
            tarGetText->SetText("FPS: 0");
            tarGetText->SetColor(Colors::BLUE);
            //tarGetText->SetBox(true);
            tarGetText->SetOutline(1);
            tarGetText->SetBackgroundColor(Colors::GREEN);
            tarGetText->TextToTexture(mainRenderer, &font);
        }

        gameObject.AddComponent<Text>(&mainRenderer, 2);
        vector<Text*> compList = gameObject.GetComponents<Text>();
        if (!compList.empty() && compList.size() > 1)
        {
            compList[1]->SetColor(Colors::RED);
            compList[1]->SetText("Current master mute key is " + (string)SDL_GetKeyName(currentKey));
            compList[1]->TextToTexture(mainRenderer, &font, 36);
            mainText = compList[1];
        }

        ///
        /// ECS + Sprite animation demo
        ///

        AnimatorTimeline timeline;

        timeline.AddTweeningFuncs((vector<CurveFunction>){Tweening::Power2, Tweening::Power3, Tweening::Power4, Tweening::Power5, Tweening::Sine, Tweening::SineHalf, Tweening::SineQuarter,
                                  Tweening::Cosine, Tweening::CosineHalf, Tweening::CosineQuarter, Tweening::Overshoot});

        SpriteAnimation spriteAnim;
        /// We also cache the image as we want to revert the texture each frame, which is costly but allows fancy real time effects...
        spriteAnim.LoadAndInit("assets/sprite_test.osa", mainRenderer, SDL_GetWindowPixelFormat(mainWindow.GetWindow()), true);

        Entity& other = *entitySystem.CreateEntity();
        other.AddComponent<Sprite>(&mainRenderer);

        Sprite* sprite = other.GetComponent<Sprite>();
        if (sprite != nullptr)
        {
            sprite->PlayAnimation(timeline, &spriteAnim, 0, -1, false);
            sprite->position.x = (float)(mainRenderer.GetWidth() / 2);
            sprite->position.y = (float)(mainRenderer.GetHeight() / 2);
        }

        ///
        /// Input handling demo
        ///

        InputContext mainContext;
        KeyboardHandler& keyboard = *mainContext.AddHandler<KeyboardHandler>();

        keyboard.AddAction("TOGGLE MUTE", *KeyAction);
        keyboard.BindAction("TOGGLE MUTE", SDLK_m);
        keyboard.AddBindlessAction(*GetKey);

        MouseHandler& mouse = *mainContext.AddHandler<MouseHandler>();

        mouse.AddAction("mouseclick", *MouseClickActionOutcome);
        mouse.AddAction("scroll", *MouseScrollActionOutcome);
        mouse.BindAction("mouseclick", MOUSE_BUTTON_LEFT);
        mouse.BindAction("scroll", MOUSE_WHEEL);

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
        SoundStream.Play("assets/test_stream.wav", 0.5f, -1);

        master.Mute();
        /// Source goes into sfx bus
        source.Link(&sfx);
        /// The sfx bus goes into the master bus
        sfx.Link(&master);
        sfx.SetVolume(0);
        if (!sound.Load("assets/test_audio.wav"))
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
        Global::delta.Init(settings);

        Point lightSpot(0.0f, 0.0f);

        debugDraw.Draw(SID("Test Draw")::str, DebugText("Hello debug draw world!", Point(mainRenderer.GetWidth() / 2, mainRenderer.GetHeight() / 2), &debugDrawEntity, &font, mainRenderer));

        world.SetDebugDraw(debugDraw.physics);

        debugDraw.physics->SetFlags(b2Draw::e_shapeBit);

        while (!quit)
        {
            /// Input handling phase
            while (SDL_PollEvent(&e) != 0)
            {
                mainWindow.HandleEvents(e);
                if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
                {
                    quit = true;
                    break;
                }
                mainInput.HandleEvent(e);
            }

            SDL_Rect viewrect = mainRenderer.GetViewportRect();

            int mx, my;
            SDL_GetMouseState(&mx, &my);
            mx -= viewrect.x;
            my -= viewrect.y;

            debugDraw.Clear(SID("Test lines")::str);
            debugDraw.Draw(SID("Test lines")::str, DebugLine(Point(mx, 0), Point(mx, mainRenderer.GetHeight())));
            debugDraw.Draw(SID("Test lines")::str, DebugLine(Point(0, my), Point(mainRenderer.GetWidth(), my), Colors::BLUE));

            lightSpot = sprite->ScreenToLocalPoint(Point((float)mx, (float)my));

            timeline.Update(Global::delta.Time());

            /// Logic update phase
            if (volume_change)
            {
                source.SetPanning(panning);
                volume_change = false;
            }

            sfx.FadeIn(Global::delta.Time(), 3.0f);
            /// Demo dynamic key binding
            if (update_binding)
            {
                /// Rebind the key
                keyboard.BindAction("TOGGLE MUTE", currentKey);
                update_binding = false;
            }

            world.Step(Global::delta.Time(), velocityIterations, positionIterations);

            //SDL_Log("dtime is %f before update", Global::delta.Time());
            entitySystem.UpdateComponents();
            //SDL_Log("dtime is %f after update", global::delta.Time());
            /// TODO: for some reason this bit crashes the engine after a brief time. Something to do with the animator.
            //spriteAnim.Init(mainRenderer, SDL_GetWindowPixelFormat(mainWindow.GetWindow()), true);

            /// Pixel-precise lighting effect that stays constant despite sprite size changes
            //SDL_Rect area = sprite->GetClip();
            /*spriteAnim.ApplyEffect([&lightSpot, &sprite] (SDL_Color c, SDL_Point p) {
                float brightness = 1.0f - mapRange(clamp(lightSpot.DistanceSquared((Point){(float)p.x, (float)p.y}), 0.0f, (96.0f / sprite->GetRenderWidth()) * (96.0f / sprite->GetRenderWidth())), 0.0f, (100.0f / sprite->GetRenderWidth()) * (100.0f / sprite->GetRenderWidth()), 0.0f, 1.0f);
                c = Color((Uint8)(brightness * (float)c.r), (Uint8)(brightness * (float)c.g), (Uint8)(brightness * (float)c.b), c.a);
                return c;
            });*/

            /// Rendering phase
            SDL_RenderClear(mainRenderer.GetRendererSDL());

            vector<Text*> handyComponents = gameObject.GetComponents<Text>();
            if (!handyComponents.empty())
            {
                for (int i = 0, counti = handyComponents.size(); i < counti; i++)
                {
                    handyComponents[i]->position.x = (handyComponents[i]->GetWidth() / 2) + posx;
                    handyComponents[i]->position.y = (handyComponents[i]->GetHeight() / 2) + (i * 50) + posy;
                }
                if (fpsTimer.GetTicks() > 250)
                {
                    handyComponents[0]->SetText("FPS: " + ToString((int)(countedFrames / (fpsTimer.GetTicks() / 1000.0f))));
                    countedFrames = 0;
                    fpsTimer.Start();
                }
            }
            world.DrawDebugData();

            mainRenderer.RenderPresent(true);

            mainRenderer.SetDrawColor(Colors::RED);
            viewrect.x = 0;
            viewrect.y = 0;
            SDL_RenderDrawRect(mainRenderer.GetRendererSDL(), &viewrect);

            entitySystem.DestroyPending();
            mainRenderer.SetDrawColor(Colors::BLACK);
            SDL_RenderPresent(mainRenderer.GetRendererSDL());

            /// Update timer and FPS count
            countedFrames++;
            Global::delta.Update();
        }
    }
    TerminateOssium();
    return 0;
}
