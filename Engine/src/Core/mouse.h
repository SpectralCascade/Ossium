/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
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
#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"
#include "coremaths.h"

namespace Ossium
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
    struct OSSIUM_EDL MouseInput
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

    class OSSIUM_EDL MouseHandler : public InputHandler<MouseHandler, MouseInput, MouseEvent>
    {
    public:
        ActionOutcome HandleInput(const SDL_Event& raw);

        /// Returns the current mouse position relative to the currently set viewport.
        Vector2 GetMousePosition();

        /// Returns the absolute mouse position on screen.
        static Vector2 GetAbsoluteMousePosition();

        bool LeftPressed();
        bool RightPressed();
        bool MiddlePressed();

        /// Sets the relative viewport so that the reported mouse position is offset appropriately, rather than reporting mouse position relative to the native window.
        void SetViewport(SDL_Rect viewport);
        /// Returns the relative viewport
        SDL_Rect GetViewport();

    private:
        bool leftButtonPressed = false;
        bool rightButtonPressed = false;
        bool middleButtonPressed = false;

        SDL_Rect viewportRect;

    };

}

#endif // MOUSE_H
