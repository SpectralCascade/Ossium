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
        void ListScene(SceneHierarchyItem& item, bool loaded);
        void ListEntity(Entity* entity);

        // The selected entity, if any.
        Entity* selectedEntity = nullptr;
        std::string selectedScene;

    };

}

#endif // ENTITYHIERARCHY_H
