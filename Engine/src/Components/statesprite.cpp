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
#include <map>
#include <utility>
#include <string>

#include "texture.h"
#include "statesprite.h"
#include "../Core/renderer.h"

using namespace std;

namespace Ossium
{

    ///
    /// StateSpriteTable
    ///

    string StateSpriteTable::ToString()
    {
        stringstream data;
        data.str("");
        data << "(";
        for (auto itr = begin(); itr != end(); itr++)
        {
            if (itr->second.first != nullptr)
            {
                data << itr->first << " [" << itr->second.first->GetPathName() << ", " << itr->second.second << "]," << endl;
            }
        }
        data << ")";
        return data.str();
    }

    void StateSpriteTable::FromString(const string& data)
    {
        bool keymode = false;
        string valueStr;
        string key = "";
        pair<Image*, Uint16> value;
        for (int i = 1, counti = data.length(); i < counti - 1; i++)
        {
            if (data[i] == '[')
            {
                if (keymode)
                {
                    key = valueStr;
                }
                keymode = false;
                valueStr = "";
            }
            else if (data[i] != ',' && data[i] != ']')
            {
                valueStr += data[i];
            }
            else if (!keymode)
            {
                if (data[i] == ',')
                {
                    Image* loadedImage = resources->Get<Image>(valueStr, *renderer);
                    value.first = loadedImage;
                }
                else
                {
                    Utilities::FromString(value.second, valueStr);
                    ((*this)[key]) = value;
                }
                valueStr = "";
            }
        }
    }

    ///
    /// StateSprite
    ///

    REGISTER_COMPONENT(StateSprite);

    void StateSprite::OnCreate()
    {
        states.renderer = GetService<Renderer>();
        states.resources = GetService<ResourceController>();
    }

    void StateSprite::OnLoadFinish()
    {
        // Skip texture OnLoadFinish() because the state should be setup manually
        ParentType::OnLoadFinish();
    }

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
