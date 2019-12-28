/** @file */
#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <Ossium.h>

#include "../Core/neurongui.h"

using namespace Ossium;

namespace Ossium::Editor
{

    /// Contains layout data for an EditorWindow.
    struct EditorWindowSchema : public Schema<EditorWindowSchema, 20>
    {
        DECLARE_BASE_SCHEMA(EditorWindowSchema, 20);

    };

    /// A dockable window with support for custom Immediate-Mode GUI.
    /**
     * TODO: MAKE THIS A COMPONENT
     * This component allows developers to create custom editor windows by inheritance.
     * You can create custom UI within the window by overriding the virtual OnGUI method and calling
     * Immediate-Mode GUI methods.
     * EditorWindow instances can be docked within the main editor window, or undocked as a native OS window.
     */
    class EditorWindow : public NeuronGUI, public EditorWindowSchema
    {
    private:
        /// The native OS window.
        Window* native = nullptr;



        /// Initialises the window.
        //void OnLoadFinish();

    public:
        //DECLARE_COMPONENT(Transform, EditorWindow);
        CONSTRUCT_SCHEMA(NeuronGUI, EditorWindowSchema);

        /// TODO: remove me
        EditorWindow(Renderer* render, InputContext* inputContext, ResourceController* resourceController) :
            NeuronGUI(render, inputContext, resourceController) { }

        /// Override this method in a derived component and use custom Immediate-Mode GUI.
        virtual void OnGUI();

        /// Forces the GUI to update and re-render.
        //void Refresh();

        /// Closes the window.
        //void Close();

    };

}

#endif // EDITORWINDOW_H
