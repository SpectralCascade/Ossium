#include "teststuff.h"
#include "Ossium/window.h"

using namespace Ossium;

REGISTER_COMPONENT(StickFighter);

void StickFighter::OnCreate()
{
    context = new InputContext();
    keyboard = context->AddHandler<KeyboardHandler>();
    keyboard->AddAction("left", [&](const KeyboardInput& data){ this->OnMoveLeft(data); });
    keyboard->AddAction("right", [&](const KeyboardInput& data){ this->OnMoveRight(data); });
    keyboard->AddAction("up", [&](const KeyboardInput& data){ this->OnMoveUp(data); });
    keyboard->AddAction("down", [&](const KeyboardInput& data){ this->OnMoveDown(data); });
    keyboard->Bind("left", SDLK_LEFT);
    keyboard->Bind("right", SDLK_RIGHT);
    keyboard->Bind("up", SDLK_UP);
    keyboard->Bind("down", SDLK_DOWN);
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

void StickFighter::OnMoveLeft(const KeyboardInput& data)
{
    if (data.state == KEY_DOWN)
    {
        movement.x = movement.x == 0 ? -5 : 0;
    }
    else
    {
        movement.x = 0;
    }
}

void StickFighter::OnMoveRight(const KeyboardInput& data)
{
    if (data.state == KEY_DOWN)
    {
        movement.x = movement.x == 0 ? 5 : 0;
    }
    else
    {
        movement.x = 0;
    }
}

void StickFighter::OnMoveUp(const KeyboardInput& data)
{
    if (data.state == KEY_DOWN)
    {
        movement.y = movement.y == 0 ? -5 : 0;
    }
    else
    {
        movement.y = 0;
    }
}

void StickFighter::OnMoveDown(const KeyboardInput& data)
{
    if (data.state == KEY_DOWN)
    {
        movement.y = movement.y == 0 ? 5 : 0;
    }
    else
    {
        movement.y = 0;
    }
}

void StickFighter::Update()
{
    timeline.Update(global::delta.time());
    //global::delta.update();
    stickman->position += movement;
}
