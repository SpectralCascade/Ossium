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
#ifndef STATESPRITE_H
#define STATESPRITE_H

#include <map>
#include <utility>
#include <string>

#include "../Components/texture.h"
#include "../Core/resourcecontroller.h"
#include "../Core/renderer.h"
#include "../Core/schemamodel.h"

namespace Ossium
{
    /// Bit masks for flag that decides whether a state texture is split along x or y axis
    /// For use with Uint16 variables, so 15 bits are freely available
    #define STATE_HORIZONTAL        32768
    #define STATE_VERTICAL              0

    /// Key = state, pair first = pointer to texture resource, pair second = clipping info
    /// First 15 bits of clipping info = number of segments
    /// Final bit decides whether clipping along horizontal or vertical
    /// This is in a separate class so it can implement the ToString() and FromString methods,
    /// which are required for schema serialisation because maps and pairs are not supported at the time of writing
    class OSSIUM_EDL StateSpriteTable : public std::map<std::string, std::pair<Image*, Uint16>>
    {
    public:
        std::string ToString();
        void FromString(const std::string& data);

        Renderer* renderer = nullptr;
        ResourceController* resources = nullptr;
    };

    struct OSSIUM_EDL StateSpriteSchema : public Schema<StateSpriteSchema>
    {
        DECLARE_SCHEMA(StateSpriteSchema, Schema<StateSpriteSchema>);

        /// Current state key
        M(std::string, currentState) = "";

        /// Current substate
        M(Uint16, currentSubState) = 0;

        /// Current horizontal flag
        M(bool, horizontalFlag) = true;

        /// Current number of segments
        M(Uint16, totalCurrentSegments) = 1;

    };

    /// Can be switched between different textures/texture clips
    class OSSIUM_EDL StateSprite : public IOnButtonState, public Texture, protected StateSpriteSchema
    {
    public:
        CONSTRUCT_SCHEMA(Texture, StateSpriteSchema);
        DECLARE_COMPONENT(Texture, StateSprite);

        /// Initialise states table
        void OnCreate();

        /// Switch to primary state.
        void OnLoadFinish();

        /// Adds a state to the sprite; horizontal specifies whether the image should be sliced horizontally or vertically,
        /// and segments specifies how many substates the image should be sliced up into.
        bool AddState(std::string state, Image* image, bool horizontal = true, Uint16 segments = 1);

        /// Changes state and initialises current substate to 0
        bool ChangeState(const std::string& state);

        /// Changes current substate clip segment
        void ChangeSubState(Uint16 substate, bool forceChange = false);

        /// Returns the string id of the current state
        std::string GetCurrentState();

        /// Returns the current substate segment number
        Uint16 GetCurrentSubstate();

        /// Returns the current substate clip rect
        SDL_Rect GetCurrentClip();

        /// Implementation of IOnButtonState.
        void OnButtonState(int state);

        /// Multiple states, multiple textures
        StateSpriteTable states;

    protected:
        /// Original addState method - this one does worry about bit masks, hence why it's abstracted away
        bool AddState(std::string state, Image* image, Uint16 clipData = 1 | STATE_HORIZONTAL);

    };

}

#endif // STATESPRITE_H
