#ifndef STATESPRITE_H
#define STATESPRITE_H

#include <map>
#include <utility>
#include <string>
#include <SDL.h>

#include "texture.h"
#include "resourcecontroller.h"
#include "renderer.h"

using namespace std;

namespace Ossium
{
    /// Bit masks for flag that decides whether a state texture is split along x or y axis
    /// For use with Uint16 variables, so 15 bits are freely available
    #define STATE_HORIZONTAL        32768
    #define STATE_VERTICAL              0

    inline namespace graphics
    {

        /// Can be switched between different textures/texture clips
        class StateSprite : public Texture
        {
        public:
            DECLARE_COMPONENT(StateSprite);

            /// Adds a state to the sprite; horizontal specifies whether the image should be sliced horizontally or vertically,
            /// and segments specifies how many substates the image should be sliced up into.
            bool AddState(string state, Image* image, bool horizontal = true, Uint16 segments = 1);

            /// Changes state and initialises current substate to 0
            bool ChangeState(const string& state);

            /// Changes current substate clip segment
            void ChangeSubState(Uint16 substate, bool forceChange = false);

            /// Returns the string id of the current state
            string GetCurrentState();

            /// Returns the current substate segment number
            Uint16 GetCurrentSubstate();

            /// Returns the current substate clip rect
            SDL_Rect GetCurrentClip();

        protected:
            /// Original addState method - this one does worry about bit masks, hence why it's abstracted away
            bool AddState(string state, Image* image, Uint16 clipData = 1 | STATE_HORIZONTAL);

            /// Multiple states, multiple textures
            /// Key = state, pair first = pointer to texture resource, pair second = clipping info
            /// First 15 bits of clipping info = number of segments
            /// Final bit decides whether clipping along horizontal or vertical
            map<string, pair<Image*, Uint16>> states;

            /// Current state key
            string currentState = "";

            /// Current substate
            Uint16 currentSubState = 0;

            /// Current horizontal flag
            bool horizontalFlag = true;

            /// Current number of segments
            Uint16 totalCurrentSegments = 1;

        };

    }

}

#endif // STATESPRITE_H
