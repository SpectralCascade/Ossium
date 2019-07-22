#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"
#include "coremaths.h"

namespace Ossium
{

    inline namespace Input
    {

        enum MouseEvent
        {
            MOUSE_UNKNOWN = 0,
            MOUSE_MOTION,
            MOUSE_BUTTON_LEFT,
            MOUSE_BUTTON_RIGHT,
            MOUSE_BUTTON_MIDDLE,
            MOUSE_BUTTON_OTHER,
            MOUSE_WHEEL
        };

        const Uint8 MOUSE_PRESSED = SDL_PRESSED;
        const Uint8 MOUSE_RELEASED = SDL_RELEASED;

        /// Unlike a keyboard, a mouse has more than one type of input (scroll wheel etc.)
        struct MouseInput
        {
            /// Type of mouse event
            MouseEvent type;
            /// Which mouse button is associated with this event
            Uint8 button;
            /// X position of mouse relative to window, OR amount scrolled in x direction
            Sint32 x;
            /// Y position of mouse relative to window, OR amount scrolled in y direction
            Sint32 y;
            /// X position of mouse relative to previous x position (if mouse motion event)
            Sint32 xrel;
            /// Y position of mouse relative to previous y position (if mouse motion event)
            Sint32 yrel;
            /// Whether the associated button is pressed, OR button bit mask as returned by SDL_GetMouseState()
            Uint8 state;
            /// Number of clicks, 1 = single click, 2 = double click, etc.
            Uint8 clicks;
            /// Direction of the scroll wheel (SDL_MOUSEWHEEL_NORMAL or SDL_MOUSEWHEEL_FLIPPED)
            Uint32 direction;
        };

        class MouseHandler : public InputHandler<MouseInput, MouseEvent>
        {
        public:
            DECLARE_INPUT_HANDLER(MouseHandler);

            ActionOutcome HandleInput(const SDL_Event& raw);

        };

    }

}

#endif // MOUSE_H
