#ifndef TESTSTUFF_H
#define TESTSTUFF_H

//#include "Ossium/Components/sprite.h"
#include "Ossium/Core/keyboard.h"
#include "Ossium/Core/delta.h"
#include "Ossium/Core/component.h"

using namespace std;
using namespace Ossium;

/*
/// Note this entire class is just for messing around. Don't follow this as an example for structuring your code!
class StickFighter : public Component
{
public:
    DECLARE_COMPONENT(StickFighter);

    void OnCreate();

    void OnInit(Renderer* renderer, int layer);

    void OnDestroy();

    void Update();

    InputContext* context;

private:
    Vector2 movement;

    Sprite* stickman;

    SpriteAnimation idleAnim;
    SpriteAnimation walkAnim;

    AnimatorTimeline timeline;

    KeyboardHandler* keyboard;

};

*/

class MouseFollower : public Component
{
public:
    DECLARE_COMPONENT(MouseFollower);

    void Update();

};

#endif // TESTSTUFF_H
