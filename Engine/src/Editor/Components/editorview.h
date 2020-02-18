/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *  
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  
 *  3. This notice may not be removed or altered from any source distribution.
 *  
**/
/** @file */
#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "../../Ossium.h"

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
