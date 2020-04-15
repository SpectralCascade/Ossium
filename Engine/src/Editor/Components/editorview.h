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
#include "../../Core/tree.h"

using namespace Ossium;

namespace Ossium::Editor
{

    /// Contains layout data for an EditorWindow.
    struct EditorWindowSettings : public Schema<EditorWindowSettings, 20>
    {
        DECLARE_BASE_SCHEMA(EditorWindowSettings, 20);

        M(string, title) = "Untitled";

    };

    enum DockingMode
    {
        TOP = 0,
        BOTTOM,
        LEFT,
        RIGHT
    };

    /// Forward declaration
    class NativeEditorWindow;

    /// A dockable window with support for custom Immediate-Mode GUI.
    /**
     * This is used to create custom editor windows (via inheritance).
     * You can create custom UI within the window by overriding the virtual OnGUI method and calling
     * Immediate-Mode GUI methods.
     * EditorWindow instances can be docked within the main native window. Currently they cannot be undocked as a native OS window however.
     */
    class EditorWindow : public NeuronGUI
    {
    private:
        friend class NativeEditorWindow;

        /// Settings defining the window title, dimensions etc.
        EditorWindowSettings settings;

        /// The corresponding node of this window.
        Node<EditorWindow*>* node;

        InputController* inputController;

        NativeEditorWindow* native = nullptr;

    protected:
        EditorWindow() = default;

    public:
        virtual ~EditorWindow();

        /// Initialises input
        void Init(InputController* inputControl, ResourceController* resourceController, NativeEditorWindow* nativeWindow);

        /// Returns the native editor window that owns this editor window.
        NativeEditorWindow* GetNativeWindow();

        /// Sets the window title.
        void SetTitle(string title);

        /// Returns the window title.
        string GetTitle();

        /// Closes this editor window.
        void Close();

    };

    /// This holds the actual native editor window and the layout tree for docked editor windows.
    class NativeEditorWindow
    {
    private:
        /// The renderer to use for this window.
        Renderer* renderer = nullptr;

        /// The input context for the native window.
        InputContext* windowContext = nullptr;

        /// The input controller pointer.
        InputController* input;

        /// Minimum dimensions of the window.
        const Vector2 MIN_DIMENSIONS = {64, 64};

        /// Layout tree, consisting of alternating row-column-row (or column-row-column, depending on root layout).
        Tree<EditorWindow*> layout;

        /// The native OS window.
        Window* native = nullptr;

        bool layoutRow = 0;
        bool layoutColumn = 1;

        /// Docks an existing source editor window to a destination editor window.
        void Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode);

        /// Everything in the window is rendered to this texture instead of using the renderer directly to avoid double buffering issues.
        SDL_Texture* renderBuffer = NULL;

    public:
        /// Creates the window and initialises the tree.
        NativeEditorWindow(InputController* controller, int w = 480, int h = 640);
        virtual ~NativeEditorWindow();

        /// Update editor windows.
        void Update();

        /// Instantiates a specific editor window instance and initialises it, then inserts within the layout tree.
        template<typename T>
        typename enable_if<is_base_of<EditorWindow, T>::value, T*>::type
        Add(InputController* inputControl, ResourceController* resourceController, EditorWindow* dest = nullptr, DockingMode mode = DockingMode::TOP)
        {
            T* toAdd = new T();
            toAdd->Init(inputControl, resourceController, this);
            if (dest != nullptr)
            {
                Insert(toAdd, dest, mode);
            }
            else
            {
                toAdd->node = layout.Insert(toAdd);
                toAdd->renderer = renderer;
                toAdd->Init(inputControl, resourceController, this);
                toAdd->viewport = {0, 0, native->GetWidth(), native->GetHeight()};
            }
            return toAdd;
        }

        /// Removes an editor window from this native editor window.
        void Remove(EditorWindow* source);

    };

}

#endif // EDITORWINDOW_H
