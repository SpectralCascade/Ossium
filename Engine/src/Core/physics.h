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
#ifndef PHYSICS_H
#define PHYSICS_H

#include "coremaths.h"

namespace Ossium
{

    inline float PTM(float pixels)
    {
        return pixels * 0.02f;
    }

    inline float MTP(float metres)
    {
        return metres * (1.0f / 0.02f);
    }

    inline Vector2 PTM(const Vector2& vecPixels)
    {
        return Vector2(PTM(vecPixels.x), PTM(vecPixels.y));
    }

    inline Vector2 MTP(const Vector2& vecMetres)
    {
        return Vector2(MTP(vecMetres.x), MTP(vecMetres.y));
    }

}

#endif // PHYSICS_H
