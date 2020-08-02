#ifndef EDITORCONTROLLER_H
#define EDITORCONTROLLER_H

#include "../../Ossium.h"
#include "editorstyle.h"
#include "editorlayout.h"
#include "editorconstants.h"

namespace Ossium::Editor
{

    // Forward declarations
    class ToolBar;
    class Project;

    /// This is the core of the editor at runtime.
    class EditorController : public EditorTheme
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, EditorTheme);

        /// Constructor instantiates the main window and caches a reference to the resources.
        EditorController(ResourceController* resources);
        virtual ~EditorController();

        /// Creates a new editor layout, and adds the specified editor window type to it.
        template<typename RootWindowType>
        EditorLayout* AddLayout()
        {
            EditorLayout* layout = CreateLayout();
            layout->Add<RootWindowType>(DockingMode::TOP);
            return layout;
        }

        /// Removes a specific editor layout.
        void RemoveLayout(EditorLayout* layout);

        /// Returns a pointer to the main resource controller for the editor.
        ResourceController* GetResources();

        /// Returns a pointer to the main input controller for the editor.
        InputController* GetInput();

        /// Returns the main window layout.
        EditorLayout* GetMainLayout();

        /// Operates all editor logic. Returns true if the application should quit.
        bool Update();

        /// Returns the loaded project, or nullptr if no project is loaded.
        Project* GetProject();

        /// Note: this frees the current project and opens a new, empty project.
        Project* CreateProject();

        /// Loads a specified project file.
        Project* OpenProject(std::string path);

        /// Frees the current project.
        void CloseProject();

        /// Adds a custom menu option to the tool bar. Optionally
        void AddCustomMenu(std::string menuPath, std::function<void()> onClick, std::function<bool()> isEnabled = [] () { return true; });

        /// Adds a custom menu option to the tool bar that opens a window.
        template<typename T>
        void AddToolWindow(std::string menuPath)
        {
            // TODO: if already opened, simply focus the layout window.
            AddCustomMenu(menuPath, [&] () { AddLayout<T>(); });
        }

        struct MenuTool
        {
            MenuTool(std::string path, std::function<void()> onClick, std::function<bool()> isEnabled);
            std::string path;
            std::function<void()> onClick;
            std::function<bool()> isEnabled;
        };

        /// Developer-defined menu tools.
        std::vector<MenuTool> customMenuTools;

        //
        // General editor usage
        //

        /// Returns the selected entity or nullptr if none is selected.
        Entity* GetSelectedEntity();

        /// Returns the selected scene or nullptr if none is selected.
        Scene* GetSelectedScene();

        /// Select an entity.
        void SelectEntity(Entity* entity);

        /// Select a scene.
        void SelectScene(Scene* scene);

    private:
        // Helper method for a template
        EditorLayout* CreateLayout();

        ResourceController* resources = nullptr;
        InputController* input = nullptr;

        /// This is the main editor window, where the tool bar resides and the user can quit the program from this window.
        EditorLayout* mainLayout = nullptr;

        /// This is the tool bar window that provides access to all editor utilities for the user.
        ToolBar* toolbar;

        /// When false, quit the application.
        bool running = true;

        /// Additional native layouts.
        std::vector<EditorLayout*> layouts;

        /// The current project.
        Project* loadedProject = nullptr;

        /// The entity that is currently selected.
        Entity* selectedEntity = nullptr;

        /// The opened scene that is currently selected.
        Scene* selectedScene = nullptr;

    };

}

#endif // EDITORCONTROLLER_H
