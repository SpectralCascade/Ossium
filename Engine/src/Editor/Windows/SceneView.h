#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include "../Core/editorwindow.h"

namespace Ossium::Editor
{

    class SceneView : public EditorWindow
    {
    public:
        void OnInit();

        void OnGUI();

    };

}

#endif // SCENEVIEW_H
