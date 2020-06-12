#ifndef LAYOUT_DIAGRAM_H
#define LAYOUT_DIAGRAM_H

#include "../../OssiumEditor.h"

namespace Ossium::Editor
{

    class LayoutDiagram : public EditorWindow
    {
    public:
        EditorWindow* target = nullptr;

    protected:
        void OnGUI();

    };

}

#endif // LAYOUT_DIAGRAM_H
