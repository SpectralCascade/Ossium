#include "mouse.h"

namespace Ossium
{

    inline namespace input
    {

        REGISTER_INPUT_HANDLER(MouseHandler);

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
                data.x = raw.motion.x;
                data.y = raw.motion.y;
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
                        break;
                    }
                    case SDL_BUTTON_RIGHT:
                    {
                        data.type = MOUSE_BUTTON_RIGHT;
                        break;
                    }
                    case SDL_BUTTON_MIDDLE:
                    {
                        data.type = MOUSE_BUTTON_MIDDLE;
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
                data.x = raw.button.x;
                data.y = raw.button.y;
                data.state = raw.button.state == SDL_PRESSED ? MOUSE_PRESSED : MOUSE_RELEASED;
            }
            else
            {
                data.type = MOUSE_UNKNOWN;
            }

            if (data.type != MOUSE_UNKNOWN)
            {
                return CallAction(data, data.type);
            }
            return ActionOutcome::Ignore;
        }

    }

}
