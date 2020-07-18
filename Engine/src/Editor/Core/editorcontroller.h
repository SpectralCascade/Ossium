#ifndef EDITORCONTROLLER_H
#define EDITORCONTROLLER_H

#include "../../Ossium.h"
#include "editorstyle.h"
#include "editorlayout.h"

namespace Ossium::Editor
{

    // Forward declarations
    class ToolBar;

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

        /// Operates all editor logic. Returns true if the application should quit.
        bool Update();

        void AddCustomMenu(string menuPath, function<void()> onClick);

        template<typename T>
        void AddToolWindow(string menuPath)
        {
            // TODO: if already opened, simply focus the layout window.
            AddCustomMenu(menuPath, [&] () { AddLayout<T>(); });
        }

        /// Developer-defined menu tools.
        map<string, function<void()>> customMenuTools;

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
        vector<EditorLayout*> layouts;

    };

}

#endif // EDITORCONTROLLER_H
