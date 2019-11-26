#ifndef METARECT_H
#define METARECT_H

#include <cmath>

#include "schemamodel.h"
#include "coremaths.h"
#include "ecs.h"

namespace Ossium
{

    struct OSSIUM_EDL MetaRectSchema : public Schema<MetaRectSchema>
    {
        DECLARE_SCHEMA(MetaRectSchema, Schema<MetaRectSchema>);

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

        /// Returns the SDL_Rect equivalent of the rect
        SDL_Rect GetSDL(Point& position)
        {
            SDL_Rect output = {(int)round(position.x - (width * 0.5f)), (int)round(position.y - (height * 0.5f)), (int)round(width), (int)round(height)};
            return output;
        }

        Rect GetRect(Point& position)
        {
            return Rect(position.x - (width * 0.5f), position.y - (height * 0.5f), width, height);
        }

    };

}

#endif // METARECT_H
