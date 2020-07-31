#ifndef SCENEHIERARCHY_H
#define SCENEHIERARCHY_H

#include "../Core/editorwindow.h"

namespace Ossium
{
    // Forward declarations
    class Scene;
    class Entity;
}

namespace Ossium::Editor
{

    struct SceneHierarchyItem : public Schema<SceneHierarchyItem, 4>
    {
        DECLARE_BASE_SCHEMA(SceneHierarchyItem, 4);

        M(string, name);
        M(string, path);
        M(bool, opened) = true;
    };

    struct SceneHierarchySchema : public Schema<SceneHierarchySchema, 20>
    {
        DECLARE_BASE_SCHEMA(SceneHierarchySchema, 20);

    protected:
        // All associated scenes, whether loaded or not.
        M(vector<SceneHierarchyItem>, usedScenes);

    };

    class SceneHierarchy : public EditorWindow, public SceneHierarchySchema
    {
    public:
        CONSTRUCT_SCHEMA(EditorWindow, SceneHierarchySchema);

        void OnInit();

        void OnGUI();

    protected:
        void ListScene(SceneHierarchyItem& item, bool loaded);
        void ListEntity(Entity* entity);

        // The selected entity, if any.
        Entity* selectedEntity = nullptr;
        string selectedScene;

    };

}

#endif // ENTITYHIERARCHY_H
