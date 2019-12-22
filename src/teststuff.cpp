/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
 *  
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  
 *  3. This notice may not be removed or altered from any source distribution.
 *  
**/
#include "teststuff.h"
#include "Ossium/Core/window.h"
#include "Ossium/Core/pixeleffects.h"

using namespace Ossium;

/*
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
    keyboard->AddAction("squish", [&](const KeyboardInput& data) {
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
    keyboard->BindAction("squish", SDLK_LCTRL);
    Log.Info("Created a stickfighter instance.");
    OnInit(GetService<Renderer>(), -1);
}

void StickFighter::OnInit(Renderer* renderer, int layer)
{
    /// We add the sprite component here so it automagically gets registered with the renderer.
    stickman = entity->AddComponent<Sprite>();
    stickman->WorldPosition() = Point(renderer->GetWindow()->GetWidth() / 3, renderer->GetWindow()->GetHeight() / 2);
    /// We can also load and initialise the animations here.
    idleAnim.LoadAndInit("assets/stick_idle.osa", *renderer, SDL_PIXELFORMAT_ARGB8888);
    walkAnim.LoadAndInit("assets/stick_walk.osa", *renderer, SDL_PIXELFORMAT_ARGB8888);
    stickman->PlayAnimation(timeline, &idleAnim, 0, -1);
    stickman->SetBlendMode(SDL_BLENDMODE_BLEND, true);
    idleAnim.ApplyEffect(Graphics::InvertColor);
    stickman->PlayAnimation(timeline, &idleAnim, 0, -1);
    stickman->SetBlendMode(SDL_BLENDMODE_BLEND, true);
    walkAnim.ApplyEffect(Graphics::InvertColor);
}

void StickFighter::OnDestroy()
{
    delete context;
    context = nullptr;
    Log.Info("Destroying a stickfighter instance now.");
}

void StickFighter::Update()
{
    timeline.Update(Global::delta.Time());
    bool moving = false;
    if (keyboard->GetState("up"))
    {
        stickman->WorldPosition().y -= 5;
        moving = true;
    }
    if (keyboard->GetState("down"))
    {
        stickman->WorldPosition().y += 5;
        moving = true;
    }
    if (keyboard->GetState("left"))
    {
        stickman->WorldPosition().x -= 5;
        moving = true;
    }
    if (keyboard->GetState("right"))
    {
        stickman->WorldPosition().x += 5;
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
*/

REGISTER_COMPONENT(MouseFollower);

void MouseFollower::Update()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    GetService<Renderer>()->WindowToViewportPoint(x, y);
    WorldPosition() = Point(x, y);
}
