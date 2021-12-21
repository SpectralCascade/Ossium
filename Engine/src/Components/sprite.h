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
#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"

namespace Ossium
{

    class Sprite : public Texture
    {
    public:
        DECLARE_COMPONENT(Texture, Sprite);

        /// Overrides Texture to account for offsets.
        void SetRenderWidth(float percent);
        /// Overrides Texture to account for offsets.
        void SetRenderHeight(float percent);

        /// Overrides Texture to account for percent width and percent height.
        void SetClip(int x, int y, int w = 0, int h = 0, bool autoscale = true);

    protected:
        /// Whether or not this sprite has ever had PlayAnimation() called or not. Necessary for configuring dimensions the first time an animation is loaded.
        bool initialised = false;

        /// Stores the temporary position offset so the position can be reverted to normal when no longer animating.
        Point positionOffset = {0, 0};

        /// Used in a very similar way to positionOffset.
        Point originOffset = {0, 0};

        /// Stores the temporary width of the sprite as percentage, relative to the actual render width
        float percentWidth = 1;
        /// Stores the temporary height of the sprite as percentage, relative to the actual render height
        float percentHeight = 1;

        /// The angular offset of the sprite, used in a similar way to positionOffset
        float angleOffset = 0;

    };

}

#endif // SPRITE_H
