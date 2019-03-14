#include "teststuff.h"
#include "Ossium/window.h"
#include "Ossium/colours.h"

using namespace Ossium;

REGISTER_COMPONENT(StickFighter);

void StickFighter::OnCreate()
{
    context = new InputContext();
    keyboard = context->AddHandler<KeyboardHandler>();
    keyboard->AddState("left");
    keyboard->AddState("right");
    keyboard->AddState("up");
    keyboard->AddState("down");
    keyboard->BindState("left", SDLK_LEFT);
    keyboard->BindState("right", SDLK_RIGHT);
    keyboard->BindState("up", SDLK_UP);
    keyboard->BindState("down", SDLK_DOWN);
    keyboard->AddActionOutcome("squish", [&](const KeyboardInput& data) {
        if (data.state)
        {
            this->stickman->SetRenderHeight(0.5f);
        }
        else
        {
            this->stickman->SetRenderHeight(1.0f);
        }
        return ActionOutcome::ClaimContext;
    });
    keyboard->Bind("squish", SDLK_LCTRL);
    SDL_Log("Created a stickfighter instance.");
}

void StickFighter::OnInitGraphics(Renderer* renderer)
{
    /// We add the sprite component here so it automagically gets registered with the renderer.
    stickman = entity->AddComponent<Sprite>(renderer);
    stickman->position = Point(renderer->GetWindow()->getWidth() / 2, renderer->GetWindow()->getHeight() / 2);
    /// We can also load and initialise the animations here.
    idleAnim.LoadAndInit("stick_idle.osa", *renderer, SDL_PIXELFORMAT_ARGB8888);
    walkAnim.LoadAndInit("stick_walk.osa", *renderer, SDL_PIXELFORMAT_ARGB8888);
    stickman->PlayAnimation(timeline, &idleAnim, 0, -1);
    stickman->SetBlendMode(SDL_BLENDMODE_BLEND, true);
    idleAnim.ApplyEffect([](SDL_Color pixelData, SDL_Point pixelPos) {
        pixelData.r = pixelData.r > 127 ? 127 - (pixelData.r - 128) : 127 + (128 - pixelData.r);
        pixelData.g = pixelData.g > 127 ? 127 - (pixelData.g - 128) : 127 + (128 - pixelData.g);
        pixelData.b = pixelData.b > 127 ? 127 - (pixelData.b - 128) : 127 + (128 - pixelData.b);
        pixelData.a = pixelData.r == 0 && pixelData.g == 0 && pixelData.b == 0 ? 0x00 : 0xFF;
        return pixelData;
    });
    stickman->PlayAnimation(timeline, &idleAnim, 0, -1);
    stickman->SetBlendMode(SDL_BLENDMODE_BLEND, true);
    walkAnim.ApplyEffect([](SDL_Color pixelData, SDL_Point pixelPos) {
        pixelData.r = pixelData.r > 127 ? 127 - (pixelData.r - 128) : 127 + (128 - pixelData.r);
        pixelData.g = pixelData.g > 127 ? 127 - (pixelData.g - 128) : 127 + (128 - pixelData.g);
        pixelData.b = pixelData.b > 127 ? 127 - (pixelData.b - 128) : 127 + (128 - pixelData.b);
        pixelData.a = pixelData.r == 0 && pixelData.g == 0 && pixelData.b == 0 ? 0x00 : 0xFF;
        return pixelData;
    });
}

void StickFighter::OnDestroy()
{
    delete context;
    context = nullptr;
    SDL_Log("Destroying a stickfighter instance now.");
}

void StickFighter::Update()
{
    timeline.Update(global::delta.time());
    bool moving = false;
    if (keyboard->GetState("up"))
    {
        stickman->position.y -= 5;
        moving = true;
    }
    if (keyboard->GetState("down"))
    {
        stickman->position.y += 5;
        moving = true;
    }
    if (keyboard->GetState("left"))
    {
        stickman->position.x -= 5;
        moving = true;
    }
    if (keyboard->GetState("right"))
    {
        stickman->position.x += 5;
        moving = true;
    }
    if (moving && stickman->anim.GetSourceName() == "idle")
    {
        stickman->PlayAnimation(timeline, &walkAnim);
    }
    else if (!moving && stickman->anim.GetSourceName() == "walk")
    {
        stickman->PlayAnimation(timeline, &idleAnim);
    }
}
