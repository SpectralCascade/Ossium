#ifndef TESTSTUFF_H
#define TESTSTUFF_H

#include "Ossium/sprite.h"
#include "Ossium/keyboard.h"
#include "Ossium/delta.h"

using namespace std;
using namespace Ossium;

/// Note this entire class is just for messing around. Don't follow this as an example for structuring your code!
class StickFighter : public Component
{
public:
    DECLARE_COMPONENT(StickFighter);

    void OnCreate();

    void OnInitGraphics(Renderer* renderer);

    void OnDestroy();

    void Update();

    InputContext* context;

private:
    Vector movement;

    Sprite* stickman;

    SpriteAnimation idleAnim;
    SpriteAnimation walkAnim;

    AnimatorTimeline timeline;

    KeyboardHandler* keyboard;

};

#endif // TESTSTUFF_H
