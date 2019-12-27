/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#include "pixeleffects.h"
#include "colors.h"

namespace Ossium
{

    inline namespace Graphics
    {

        SDL_Color Grayscale(SDL_Color c, SDL_Point p)
        {
            Uint8 grayscale = (Uint8)(((float)c.r * 0.3f) + ((float)c.g * 0.59f) + ((float)c.b * 0.11f));
            return Color(grayscale, grayscale, grayscale, c.a);
        }

        SDL_Color InvertColor(SDL_Color c, SDL_Point p)
        {
            c.r = c.r > 127 ? 127 - (c.r - 128) : 127 + (128 - c.r);
            c.g = c.g > 127 ? 127 - (c.g - 128) : 127 + (128 - c.g);
            c.b = c.b > 127 ? 127 - (c.b - 128) : 127 + (128 - c.b);
            c.a = c.r == 0 && c.g == 0 && c.b == 0 ? 0x00 : 0xFF;
            return c;
        }

    }

}
