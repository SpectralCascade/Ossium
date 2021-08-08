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
#ifndef METARECT_H
#define METARECT_H

#include <cmath>

#include "schemamodel.h"
#include "coremaths.h"
#include "ecs.h"

namespace Ossium
{

    struct MetaRectSchema;

    typedef Schema<MetaRectSchema, 50> MetaRectSchemaBase;

    struct OSSIUM_EDL MetaRectSchema : public MetaRectSchemaBase
    {
        DECLARE_SCHEMA(MetaRectSchema, MetaRectSchemaBase);

        /// Dimensions of the rect
        M(float, width) = 0;
        M(float, height) = 0;

        /// The origin point around which the rect rotates, as a percentage of the dimensions
        M(Point, origin);

//        M(Entity*, testEnt) = nullptr;
//
//        M(Component*, testComp) = nullptr;

    };

    /// Some sort of magical rectangle with basic positioning and rotation information...
    class OSSIUM_EDL MetaRect : public MetaRectSchema
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, MetaRectSchema);

        MetaRect()
        {
            origin.x = 0.5f;
            origin.y = 0.5f;
        }

        /// Returns the SDL_Rect equivalent of the rect (with rounding).
        SDL_Rect GetSDL(const Point& position, Vector2 scale = Vector2::OneOne)
        {
            float w = round(width * scale.x);
            float h = round(height * scale.y);
            SDL_Rect output = {(int)round(position.x - round(w * 0.5f)), (int)round(position.y - round(h * 0.5f)), (int)w, (int)h};
            return output;
        }

        Rect GetRect(const Point& position)
        {
            return Rect(position.x - (width * 0.5f), position.y - (height * 0.5f), width, height);
        }

    };

}

#endif // METARECT_H
