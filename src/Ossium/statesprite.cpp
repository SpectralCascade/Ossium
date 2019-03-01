#include <map>
#include <utility>
#include <string>
#include <SDL.h>

#include "texture.h"
#include "statesprite.h"
#include "renderer.h"

namespace Ossium
{

    inline namespace graphics
    {

        REGISTER_COMPONENT(StateSprite);

        bool StateSprite::AddState(string state, Image* image, Uint16 clipData)
        {
            if (image != nullptr)
            {
                states[state] = {image, clipData};
                if (source == nullptr)
                {
                    ChangeState(state);
                }
                return true;
            }
            return false;
        }

        bool StateSprite::AddState(string state, Image* image, bool horizontal, Uint16 segments)
        {
            if (segments > STATE_HORIZONTAL)
            {
                segments = STATE_HORIZONTAL;
            }
            /// Convert segments into clipData - final bit indicating which axis to clip along
            return AddState(state, image, (Uint16)(segments | ((Uint16)horizontal * STATE_HORIZONTAL)));
        }

        bool StateSprite::ChangeState(const string& state)
        {
            auto i = states.find(state);
            if (i != states.end())
            {
                currentState = state;
                currentSubState = 0;
                SetSource(i->second.first);
                totalCurrentSegments = i->second.second;
                horizontalFlag = ((totalCurrentSegments & STATE_HORIZONTAL) == STATE_HORIZONTAL);
                if (horizontalFlag)
                {
                    /// Lop off the last bit to get the value we care about
                    totalCurrentSegments = totalCurrentSegments ^ STATE_HORIZONTAL;
                    height = height / totalCurrentSegments;
                }
                else
                {
                    width = width / totalCurrentSegments;
                }
                /// Update the substate to first substate
                ChangeSubState(0, true);
                return true;
            }
            return false;
        }

        void StateSprite::ChangeSubState(Uint16 substate, bool forceChange)
        {
            if (substate >= totalCurrentSegments)
            {
                substate = 0;
            }
            if (!forceChange && substate == currentSubState)
            {
                /// No need to do more calculations if we're already in the desired substate
                return;
            }
            if (totalCurrentSegments == 0)
            {
                totalCurrentSegments = 1;
            }
            currentSubState = substate;
            if (!horizontalFlag)
            {
                int clipWidth = (source->GetWidth() / (int)totalCurrentSegments);
                if (clipWidth < 1)
                {
                    clipWidth = 1;
                }
                clip = {clipWidth * substate, 0, clipWidth, source->GetHeight()};
            }
            else
            {
                int clipHeight = (source->GetHeight() / (int)totalCurrentSegments);
                if (clipHeight < 1)
                {
                    clipHeight = 1;
                }
                clip = {0, clipHeight * substate, source->GetWidth(), clipHeight};
            }
        }

        string StateSprite::GetCurrentState()
        {
            return currentState;
        }

        Uint16 StateSprite::GetCurrentSubstate()
        {
            return currentSubState;
        }

        SDL_Rect StateSprite::GetCurrentClip()
        {
            return clip;
        }

    }

}
