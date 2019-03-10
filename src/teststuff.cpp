#include "teststuff.h"
#include "Ossium/window.h"

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
    SDL_Log("Created a stickfighter instance.");
}

void StickFighter::OnInitGraphics(Renderer* renderer)
{
    /// We add the sprite component here so it automagically gets registered with the renderer.
    stickman = entity->AddComponent<Sprite>(renderer);
    stickman->position = Point(renderer->GetWindow()->getWidth() / 2, renderer->GetWindow()->getHeight() / 2);
    /// We can also load and initialise the animations here.
    idleAnim.LoadAndInit("stick_idle.osa", *renderer, SDL_GetWindowPixelFormat(renderer->GetWindow()->getWindow()), true);
    walkAnim.LoadAndInit("stick_walk.osa", *renderer, SDL_GetWindowPixelFormat(renderer->GetWindow()->getWindow()), true);
    stickman->PlayAnimation(timeline, &idleAnim, 0, -1);
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
    //global::delta.update();
    if (keyboard->GetState("up"))
    {
        stickman->position.y -= 5;
    }
    if (keyboard->GetState("down"))
    {
        stickman->position.y += 5;
    }
    if (keyboard->GetState("left"))
    {
        stickman->position.x -= 5;
    }
    if (keyboard->GetState("right"))
    {
        stickman->position.x += 5;
    }
}
