#ifndef OSS_STATESPRITE_H
#define OSS_STATESPRITE_H

#include <map>
#include <utility>
#include <string>
#include <SDL2/SDL.h>

#include "oss_texture.h"
#include "oss_resourcecontroller.h"

using namespace std;

/// Bit masks for flag that decides whether a state texture is split along x or y axis
/// For use with Uint16 variables, so 15 bits are freely available
#define OSS_STATE_HORIZONTAL        32768
#define OSS_STATE_VERTICAL              0

/// Can be switched between different textures/texture clips
class OSS_StateSprite
{
public:
    OSS_StateSprite();
    ~OSS_StateSprite();

    /// Sets the final bit to whatever the boolean flag is, rather than worry about bit masks
    bool addState(string state, OSS_Texture* texture, bool horizontal = true, Uint16 segments = 1);

    /// Changes state and initialises current substate to 0
    bool changeState(string& state);

    /// Changes current substate clip segment
    void changeSubState(Uint16 substate, bool forceChange = false);

    /*  DEPRECATED
    /// Splits a state into an equally sized number of clip segments (substates),
    /// either horizontally or vertically for basic spritesheet clipping support
    /// Only use if you didn't bother setting segments when adding the state/you want to actively change
    /// the segment number because this operation can be expensive
    void splitState(string& state, Uint16 segments, bool horizontal = true);
    */

    /// Returns the current state
    string getCurrentState();

    /// Returns the current substate segment number
    Uint16 getCurrentSubstate();

    /// Renders the current state
    void render(SDL_Renderer* renderer, int x, int y, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /// Overload for simplicity
    void render(SDL_Renderer* renderer, SDL_Rect dest, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /// Very simple alternative render method
    void renderSimple(SDL_Renderer* renderer, int x, int y);

protected:
    /// Original addState method - this one does worry about bit masks, hence why it's protected
    bool addState(string state, OSS_Texture* texture, Uint16 clipData = 1 | OSS_STATE_HORIZONTAL);

    /// Multiple states, multiple textures
    /// Key = state, pair first = pointer to texture resource, pair second = clipping info
    /// First 15 bits of clipping info = number of segments
    /// Final bit decides whether clipping along horizontal or vertical
    map<string, pair<OSS_Texture*, Uint16>> states;

    /// Current state key
    string currentState;

    /// Current state texture
    OSS_Texture* stateTexture;

    /// Current substate
    Uint16 currentSubState;

    /// Current horizontal flag
    bool horizontalFlag;

    /// Current number of segments
    Uint16 totalCurrentSegments;

    /// Current substate clip rect to use
    SDL_Rect substateRect;

};

#endif // OSS_STATESPRITE_H
