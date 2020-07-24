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

#include "../Core/editorgui.h"
#include "../../Core/tree.h"

using namespace Ossium;

namespace Ossium::Editor
{

    /// Contains layout data for an EditorWindow.
    struct EditorWindowSettings : public Schema<EditorWindowSettings, 20>
    {
        DECLARE_BASE_SCHEMA(EditorWindowSettings, 20);

        M(string, title) = "Untitled";
        M(SDL_Color, borderColor) = Colors::BLACK;
        M(bool, bordered) = true;
        M(int, padding) = 6;
        M(Vector2, minDimensions) = {64, 64};
        // TODO: min size and fixed size

    };

    // Forward declarations
    class EditorLayout;
    class EditorRect;

    /// A dockable window with support for custom Immediate-Mode GUI.
    /**
     * This is used to create custom editor windows (via inheritance).
     * You can create custom UI within the window by overriding the virtual OnGUI method and calling
     * Immediate-Mode GUI methods.
     * EditorWindow instances can be docked within the main native window. Currently they cannot be undocked as a native OS window however.
     */
    class EditorWindow : public EditorGUI, public EditorWindowSettings
    {
    public:
        CONSTRUCT_SCHEMA(EditorGUI, EditorWindowSettings);

    private:
        friend class EditorLayout;

        /// Settings defining the window title, dimensions etc.
        EditorWindowSettings settings;

        /// Pointer to the native window that owns this window.
        EditorLayout* native = nullptr;

        /// The corresponding node for this window.
        Node<EditorRect>* node;

        /// The direction in which the borders are currently being dragged. Set to zero when not dragging the borders.
        Vector2 dragDir = Vector2::Zero;

    protected:
        EditorWindow() = default;

    public:
        virtual ~EditorWindow();

        /// Initialises input
        void Init(EditorLayout* nativeWindow);

        /// Returns the native editor window that owns this editor window.
        EditorLayout* GetEditorLayout();

        /// Returns the position of this editor window across all displays.
        /// Set relative = true to get the relative position of this editor window relative to it's native display.
        Vector2 GetNativePosition(bool relative = false);

        /// Sets the window title.
        void SetTitle(string title);

        /// Returns the window title.
        string GetTitle();

        /// Override of EditorGUI
        void Refresh();

        /// Returns the associated layout node for this editor window.
        Node<EditorRect>* GetLayoutNode();

        /// Closes this editor window.
        void Destroy();

    };

}

#endif // EDITORWINDOW_H
