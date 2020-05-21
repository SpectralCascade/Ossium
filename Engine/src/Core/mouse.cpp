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
#include "mouse.h"

namespace Ossium
{

    ActionOutcome MouseHandler::HandleInput(const SDL_Event& raw)
    {
        MouseInput data;
        data.button = 0;
        data.direction = SDL_MOUSEWHEEL_NORMAL;
        data.state = MOUSE_RELEASED;
        data.clicks = 0;
        data.xrel = 0;
        data.yrel = 0;
        data.x = 0;
        data.y = 0;
        if (raw.type == SDL_MOUSEMOTION)
        {
            data.type = MOUSE_MOTION;
            data.x = raw.motion.x - viewportRect.x;
            data.y = raw.motion.y - viewportRect.y;
            data.xrel = raw.motion.xrel;
            data.yrel = raw.motion.yrel;
            data.state = raw.motion.state;
        }
        else if (raw.type == SDL_MOUSEWHEEL)
        {
            data.type = MOUSE_WHEEL;
            data.direction = raw.wheel.direction;
            data.x = raw.wheel.x;
            data.y = raw.wheel.y;
            data.state = raw.button.state == SDL_PRESSED ? MOUSE_PRESSED : MOUSE_RELEASED;
        }
        else if (raw.type == SDL_MOUSEBUTTONDOWN || raw.type == SDL_MOUSEBUTTONUP)
        {
            switch (raw.button.button)
            {
                case SDL_BUTTON_LEFT:
                {
                    data.type = MOUSE_BUTTON_LEFT;
                    leftButtonPressed = raw.type == SDL_MOUSEBUTTONDOWN ? true : false;
                    break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    data.type = MOUSE_BUTTON_RIGHT;
                    rightButtonPressed = raw.type == SDL_MOUSEBUTTONDOWN ? true : false;
                    break;
                }
                case SDL_BUTTON_MIDDLE:
                {
                    data.type = MOUSE_BUTTON_MIDDLE;
                    middleButtonPressed = raw.type == SDL_MOUSEBUTTONDOWN ? true : false;
                    break;
                }
                default:
                {
                    data.type = MOUSE_BUTTON_OTHER;
                    break;
                }
            }
            data.button = raw.button.button;
            data.clicks = raw.button.clicks;
            data.x = raw.button.x - viewportRect.x;
            data.y = raw.button.y - viewportRect.y;
            data.state = raw.button.state == SDL_PRESSED ? MOUSE_PRESSED : MOUSE_RELEASED;
        }
        else
        {
            data.type = MOUSE_UNKNOWN;
        }

        if (data.type != MOUSE_UNKNOWN)
        {
            UpdateState(data.type, data.state);
            return CallAction(data, data.type);
        }
        return ActionOutcome::Ignore;
    }

    /// Returns the mouse position relative to the set viewport (within the native window the mouse is over).
    /// Returns position (-1, -1) when the context is inactive.
    Vector2 MouseHandler::GetMousePosition()
    {
        if (context->IsActive())
        {
            int x = 0, y = 0;
            SDL_GetMouseState(&x, &y);
            return Vector2((float)(x - viewportRect.x), (float)(y - viewportRect.y));
        }
        // Return invalid position when the context is inactive
        return Vector2(-10000, -10000);
    }

    Vector2 MouseHandler::GetAbsoluteMousePosition()
    {
        int x = 0, y = 0;
        SDL_GetGlobalMouseState(&x, &y);
        return Vector2((float)x, (float)y);
    }

    bool MouseHandler::LeftPressed()
    {
        return leftButtonPressed;
    }

    bool MouseHandler::RightPressed()
    {
        return rightButtonPressed;
    }

    bool MouseHandler::MiddlePressed()
    {
        return middleButtonPressed;
    }

    void MouseHandler::SetViewport(SDL_Rect viewport)
    {
        viewportRect = viewport;
    }

    SDL_Rect MouseHandler::GetViewport()
    {
        return viewportRect;
    }

}
