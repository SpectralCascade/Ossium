#ifndef PROJECT_H
#define PROJECT_H

#include "../../Core/schemamodel.h"

namespace Ossium::Editor
{

    struct ListedScene : public Schema<ListedScene, 20>
    {
        DECLARE_BASE_SCHEMA(ListedScene, 20);

        ListedScene() = default;

        ListedScene(std::string _name, std::string _path, bool _opened, bool _loaded);

        M(std::string, name);
        M(std::string, path);
        M(bool, opened);
        M(bool, loaded);
    };

    struct ProjectSchema : public Schema<ProjectSchema, 20>
    {
        DECLARE_BASE_SCHEMA(ProjectSchema, 20);

        // All opened scenes, whether loaded or not.
        M(std::vector<ListedScene>, openScenes);

    protected:
        // The name of the project.
        M(std::string, name) = "Untitled";

        // Where the project is located on disk, as an absolute path (initialised on open).
        std::string path;

    };

    class Project : public ProjectSchema
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, ProjectSchema);

        virtual ~Project() = default;

        std::string GetName();
        void SetName(std::string name);

        std::string GetPath();
        void SetPath(std::string path);

        // Saves the project with a native file dialog for the user to configure. Returns false if the user cancels.
        bool SaveAs();

    };

}

#endif // PROJECT_H
