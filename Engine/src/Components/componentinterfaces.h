#ifndef COMPONENTINTERFACES_H
#define COMPONENTINTERFACES_H

#include "../Core/coremaths.h"

namespace Ossium
{

    enum ButtonState
    {
        BUTTON_STATE_NONE = 0,
        BUTTON_STATE_HOVERED,
        BUTTON_STATE_PRESSED
    };

    // Interface for components that follow Button states.
    class IOnButtonState
    {
    public:
        virtual void OnButtonState(int state) = 0;
    };

    // Interface for components that implement a Contains() method.
    class IContainsPoint
    {
    public:
        virtual bool Contains(Point p) = 0;
    };

}

#endif // COMPONENTINTERFACES_H
