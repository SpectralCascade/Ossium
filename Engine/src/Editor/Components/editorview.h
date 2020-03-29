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
    struct EditorWindowSettings : public Schema<EditorWindowSettings, 20>
    {
        DECLARE_BASE_SCHEMA(EditorWindowSettings, 20);

        M(string, title) = "Untitled";

        M(int, width) = 640;
        M(int, height) = 480;

    };

    /// A dockable window with support for custom Immediate-Mode GUI.
    /**
     * This is used to create custom editor windows (via inheritance).
     * You can create custom UI within the window by overriding the virtual OnGUI method and calling
     * Immediate-Mode GUI methods.
     * EditorWindow instances can be docked within the main editor window, or undocked as a native OS window.
     */
    class EditorWindow : public NeuronGUI
    {
    private:
        /// The native OS window.
        Window* native = nullptr;

        /// The editor window this window is docked to. If null, this is a standalone window.
        EditorWindow* parent = nullptr;

        /// The editor windows that are children of this window.
        vector<EditorWindow*> children;

        /// The index of this window in the parent window's children.
        Uint32 childID = 0;

        using NeuronGUI::Init;

        /// Recreates the renderer and initialises the window.
        void Recreate();

        /// Recursively gets the native window that this editor window is using.
        Window* GetNativeWindow();

        /// Settings defining the window title, dimensions etc.
        EditorWindowSettings settings;

    protected:
        /// Override that does window GUI (tabs, edges of the window, etc.).
        void Refresh();

    public:
        /// This replaces the Init() method in NeuronGUI so the input context can be automagically created to ensure it's unique to this instance.
        void Init(InputController* controller, ResourceController* resourceController);

        /// The docking modes available.
        enum DockingMode {
            TABBED = 0,
            TOP,
            BOTTOM,
            LEFT,
            RIGHT
        };

        virtual ~EditorWindow();

        /// Dock to another editor window.
        void DockTo(EditorWindow* dock);

        /// Undock from the parent window.
        void Undock();

        /// Sets the window title.
        void SetTitle(string title);

        /// Returns the window title.
        string GetTitle();

        /// Returns the parent editor window which this window is docked to, or null if not docked.
        EditorWindow* GetParent();

        /// Called when this editor window is destroyed so any docked windows can also be destroyed.
        Callback<EditorWindow> OnDestroy;

    };

}

#endif // EDITORWINDOW_H
