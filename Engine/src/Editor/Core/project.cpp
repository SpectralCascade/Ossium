#include "project.h"
#include "editorconstants.h"

#include "../Core/tinyfiledialogs.h"

using namespace std;

namespace Ossium::Editor
{

    string Project::GetName()
    {
        return name;
    }

    void Project::SetName(string name)
    {
        // TODO: don't sanitise, just validate and fail if invalid.
        string sanitised = Utilities::SanitiseFilename(name);
        if (!sanitised.empty())
        {
            this->name = sanitised;
        }
        else
        {
            Log.Warning("Cannot set scene name to '{0}' because it is either empty or it only consists of invalid characters.", name);
        }
    }

    string Project::GetPath()
    {
        return path;
    }

    void Project::SetPath(string path)
    {
        // TODO: validate path
        this->path = path;
    }

    bool Project::SaveAs()
    {
        const char* filters[2] = { "*.ossium", "*" };
        const char* dest = NULL;

        dest = tinyfd_saveFileDialog(
            "Ossium | Save Project",
            path.empty() ? EDITOR_DEFAULT_DIRECTORY : path.c_str(),
            2,
            filters,
            "Ossium Project"
        );

        if (dest)
        {
            path = dest;
            Log.Info("Saving project at '{0}'.", dest);
            // Actually perform the save.
            Save(string(dest));
            return true;
        }
        else
        {
            Log.Warning("Failed to get file destination (default path: '{0}').", path.empty() ? EDITOR_DEFAULT_DIRECTORY : path.c_str());
        }
        return false;
    }

    string Project::GetAssetsPath()
    {
        return path + string("Assets") + EDITOR_PATH_SEPARATOR;
    }

}
