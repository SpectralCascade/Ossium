#ifndef DEMO_WINDOW_DOCKING_H
#define DEMO_WINDOW_DOCKING_H

#include "../../OssiumEditor.h"

namespace Ossium::Editor
{

    struct DemoDockingSchema : public Schema<DemoDockingSchema, 20>
    {
        DECLARE_BASE_SCHEMA(DemoDockingSchema, 20);

        M(bool, docked) = false;

        M(DockingMode, dockingMode) = DockingMode::RIGHT;

        M(EditorWindow*, sibling) = nullptr;

    };

    class DemoDockingWindow : public EditorWindow, DemoDockingSchema
    {
    public:
        CONSTRUCT_SCHEMA(EditorWindow, DemoDockingSchema);

        void OnGUI();

        EditorWindow* toDock = nullptr;

    };

}

#endif // DEMO_WINDOW_DOCKING_H
