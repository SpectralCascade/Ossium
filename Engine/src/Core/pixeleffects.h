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
#ifndef PIXELEFFECTS_H
#define PIXELEFFECTS_H

#include <SDL.h>

#include "helpermacros.h"

namespace Ossium
{

    inline namespace Graphics
    {

        /// Macro for short hand declarations
        #define DECLARE_PIXEL_EFFECT(NAME) OSSIUM_EDL SDL_Color NAME(SDL_Color c, SDL_Point p)

        /// Grayscale effect
        DECLARE_PIXEL_EFFECT(Grayscale);

        /// Inverts the color of a pixel
        DECLARE_PIXEL_EFFECT(InvertColor);

    }

}

#endif // PIXELEFFECTS_H
