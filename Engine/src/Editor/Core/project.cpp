#include "project.h"
#include "editorconstants.h"

#include "../Core/tinyfiledialogs.h"

#include <filesystem>

namespace Ossium::Editor
{

    std::string Project::GetName()
    {
        return name;
    }

    void Project::SetName(std::string name)
    {
        // TODO: don't sanitise, just validate and fail if invalid.
        std::string sanitised = Utilities::SanitiseFilename(name);
        if (!sanitised.empty())
        {
            this->name = sanitised;
        }
        else
        {
            Log.Warning("Cannot set scene name to '{0}' because it is either empty or it only consists of invalid characters.", name);
        }
    }

    std::string Project::GetPath()
    {
        return path;
    }

    void Project::SetPath(std::string path)
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
            path.empty() ? EDITOR_DEFAULT_DIRECTORY : (path + name + ".ossium").c_str(),
            2,
            filters,
            "Ossium Project"
        );

        if (dest)
        {
            std::filesystem::path projectPath = std::filesystem::path(std::string(dest));
            name = projectPath.stem().string();
            Log.Info("Saving project '{0}' at '{1}'.", name, dest);

            // Now setup the assets folder
            try
            {
                projectPath.remove_filename();
                path = projectPath.string();
                projectPath /= "Assets";
                std::filesystem::create_directory(projectPath);
            }
            catch (std::exception& e)
            {
                Log.Error("Exception occurred while attempting to create the project Assets directory: {0}", e);
                return false;
            }

            // Actually save the project.
            Save(dest);

            return true;
        }
        else
        {
            Log.Warning("Failed to get file destination (default path: '{0}').", path.empty() ? EDITOR_DEFAULT_DIRECTORY : path.c_str());
        }
        return false;
    }

    std::string Project::GetAssetsPath()
    {
        std::filesystem::path assets = std::filesystem::path(path);
        return (assets / std::string("Assets")).string();
    }

}
