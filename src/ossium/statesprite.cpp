#include <map>
#include <utility>
#include <string>
#include <SDL.h>

#include "texture.h"
#include "statesprite.h"
#include "renderer.h"
/*
namespace Ossium
{
    StateSprite::StateSprite()
    {
        currentState = "";
        currentSubState = 0;
        substateRect = {0, 0, 0, 0};
        stateTexture = NULL;
        horizontalFlag = true;
        totalCurrentSegments = 1;
    }

    StateSprite::~StateSprite()
    {
        states.clear();
    }

    bool StateSprite::addState(string state, Texture* texture, Uint16 clipData)
    {
        if (texture != NULL)
        {
            states[state] = {texture, clipData};
            if (stateTexture == NULL)
            {
                changeState(state);
            }
            return true;
        }
        return false;
    }

    bool StateSprite::addState(string state, Texture* texture, bool horizontal, Uint16 segments)
    {
        if (segments > STATE_HORIZONTAL)
        {
            segments = STATE_HORIZONTAL;
        }
        /// Convert segments into clipData - final bit indicating which axis to clip along
        return addState(state, texture, (Uint16)(segments | ((Uint16)horizontal * STATE_HORIZONTAL)));
    }

    bool StateSprite::changeState(string& state)
    {
        map<string, pair<Texture*, Uint16>>::iterator i = states.find(state);
        if (i != states.end())
        {
            currentState = state;
            currentSubState = 0;
            stateTexture = i->second.first;
            totalCurrentSegments = i->second.second;
            horizontalFlag = ((totalCurrentSegments & STATE_HORIZONTAL) == STATE_HORIZONTAL);
            if (horizontalFlag)
            {
                /// Lop off the last bit to get the value we care about
                totalCurrentSegments = totalCurrentSegments ^ STATE_HORIZONTAL;
            }
            /// Update the substate to first substate
            changeSubState(0, true);
            return true;
        }
        return false;
    }

    void StateSprite::changeSubState(Uint16 substate, bool forceChange)
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
            int clipWidth = (stateTexture->GetWidth() / (int)totalCurrentSegments);
            if (clipWidth < 1)
            {
                clipWidth = 1;
            }
            substateRect = {clipWidth * substate, 0, clipWidth, stateTexture->GetHeight()};
        }
        else
        {
            int clipHeight = (stateTexture->GetHeight() / (int)totalCurrentSegments);
            if (clipHeight < 1)
            {
                clipHeight = 1;
            }
            substateRect = {0, clipHeight * substate, stateTexture->GetWidth(), clipHeight};
        }
    }

    string StateSprite::getCurrentState()
    {
        return currentState;
    }

    Uint16 StateSprite::getCurrentSubstate()
    {
        return currentSubState;
    }

    void StateSprite::Render(Renderer& renderer)
    {
        stateTexture->Render(renderer);
    }


}
*/
