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
#include <algorithm>

#include "sprite.h"

namespace Ossium
{

    ///
    /// Sprite
    ///

    REGISTER_COMPONENT(Sprite);

    void Sprite::SetRenderWidth(float percent)
    {
        width = percentWidth == 0 ? 0 : (width / percentWidth);
        Texture::SetRenderWidth(percent * percentWidth);
    }

    void Sprite::SetRenderHeight(float percent)
    {
        height = percentHeight == 0 ? 0 : (height / percentHeight);
        Texture::SetRenderHeight(percent * percentHeight);
    }

    void Sprite::SetClip(int x, int y, int w, int h, bool autoScale)
    {
        if (autoScale)
        {
            /// Cache percentage width and height
            float wpercent = (clip.w == 0 ? 0 : width / (float)clip.w) * percentWidth;
            float hpercent = (clip.h == 0 ? 0 : height / (float)clip.h) * percentHeight;
            clip = {x, y, w, h};
            /// Recalculate destination dimensions with new clip rect
            Texture::SetRenderWidth(wpercent);
            Texture::SetRenderHeight(hpercent);
        }
        else
        {
            clip = {x, y, w, h};
        }
    }

}
