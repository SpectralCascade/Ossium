#ifndef SCENEHIERARCHY_H
#define SCENEHIERARCHY_H

#include "../Core/editorwindow.h"
#include "../Core/project.h"

namespace Ossium
{
    // Forward declarations
    class Scene;
    class Entity;
}

namespace Ossium::Editor
{

    class SceneHierarchy : public EditorWindow
    {
    public:
        void OnInit();

        void OnGUI();

    protected:
        void ListScene(ListedScene& item, bool loaded);
        void ListEntity(Entity* entity);

        bool rightPressed = false;
        bool rightWasPressed = false;

    };

}

#endif // ENTITYHIERARCHY_H
