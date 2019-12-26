#ifndef PANEL_H
#define PANEL_H

#include "../../Core/schemamodel.h"

namespace Ossium
{

    /// Forward declaration
    class Panel;

    struct PanelSchema : public Schema<PanelSchema, 3>
    {
    public:
        DECLARE_BASE_SCHEMA(PanelSchema, 3);

        M(Vector2, anchorMin) = Vector2(0.5f, 0.5f);
        M(Vector2, anchorMax) = Vector2(0.5f, 0.5f);

    };

    class Panel : public Transform, public PanelSchema
    {
    public:
        DECLARE_COMPONENT(Transform, Panel);
        CONSTRUCT_SCHEMA(Transform, PanelSchema);

    };

}

#endif // PANEL_H
