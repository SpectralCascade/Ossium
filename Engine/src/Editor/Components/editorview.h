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
        M(SDL_Color, borderColor) = Colors::BLACK;
        M(bool, bordered) = true;
        M(int, padding) = 6;
        // TODO: min size and fixed size

    };

    enum DockingMode
    {
        TOP             =   1,
        BOTTOM          =   2,
        LEFT            =   4,
        RIGHT           =   8,
        TOP_ADJACENT    =   16,
        BOTTOM_ADJACENT =   32,
        LEFT_ADJACENT   =   64,
        RIGHT_ADJACENT  =   128
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
    class EditorWindow : public NeuronGUI, public EditorWindowSettings
    {
    public:
        CONSTRUCT_SCHEMA(NeuronGUI, EditorWindowSettings);

    private:
        friend class EditorLayout;

        /// Settings defining the window title, dimensions etc.
        EditorWindowSettings settings;

        /// Pointer to the native window that owns this window.
        EditorLayout* native = nullptr;

        /// The corresponding node for this window.
        Node<EditorRect>* node;

    protected:
        EditorWindow() = default;

    public:
        virtual ~EditorWindow();

        /// Initialises input
        void Init(EditorLayout* nativeWindow);

        /// Returns the native editor window that owns this editor window.
        EditorLayout* GetEditorLayout();

        /// Sets the window title.
        void SetTitle(string title);

        /// Returns the window title.
        string GetTitle();

        /// Override of NeuronGUI
        void Refresh();

        /// Returns the associated layout node for this editor window.
        Node<EditorRect>* GetLayoutNode();

        /// Closes this editor window.
        void Destroy();

    };

    /// Represents a rectangular area within an editor window. May contain a specific window.
    class EditorRect : public Rect
    {
    public:
        EditorRect() = default;
        EditorRect(SDL_Rect rect) : Rect(rect) { }
        EditorRect(EditorWindow* win, SDL_Rect rect) : Rect(rect) { window = win; }

        /// The window associated with this rectangular area. This is always null for branches but never null for leaves.
        EditorWindow* window = nullptr;

    };

    /// This holds the actual native editor window and the layout tree for docked editor windows.
    class EditorLayout
    {
    private:
        /// The renderer to use for this window.
        Renderer* renderer = nullptr;

        /// The input context for the native window.
        InputContext windowContext;

        /// The input controller instance.
        InputController* input;

        /// The resource controller instance.
        ResourceController* resources;

        /// Minimum dimensions of the window.
        const Vector2 MIN_DIMENSIONS = {64, 64};

        /// Layout tree, consisting of alternating row-column-row (or column-row-column, depending on root layout).
        Tree<EditorRect> layout;

        /// The native OS window.
        Window* native = nullptr;

        /// If a node's depth % 2 == layoutRow, siblings are all columns on the left or right.
        bool layoutRow = 0;
        /// If a node's depth % 2 == layoutColumn, siblings are all rows above or below.
        bool layoutColumn = 1;

        /// Everything in the window is rendered to this texture instead of using the renderer directly to avoid double buffering issues.
        SDL_Texture* renderBuffer = NULL;

        /// Set of editor windows that are to be removed at the end of the next Update() call.
        set<EditorWindow*> toRemove;

        /// Should the viewports be updated on the next Update() call?
        bool updateViewports = false;

        /// Docks an existing source editor window to a destination editor window.
        bool Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode);

        /// Removes an editor window from this native editor window.
        bool Remove(EditorWindow* source);

    public:
        /// Creates the window and initialises the tree.
        EditorLayout(InputController* controller, ResourceController* resourceController, string title = "Untitled", int w = -1, int h = -1);
        virtual ~EditorLayout();

        /// Handles an incoming SDL_Event
        void HandleEvent(SDL_Event& e);

        /// Update editor windows.
        void Update();

        /// Return the native OS window instance
        Window* GetNativeWindow();

        /// Attempts to resize and editor window rect
        void Resize(EditorWindow* window, Rect rect);

        /// Instantiates a specific editor window instance and initialises it, then inserts within the layout tree.
        template<typename T>
        typename enable_if<is_base_of<EditorWindow, T>::value, T*>::type
        Add(EditorWindow* dest, DockingMode mode)
        {
            T* toAdd = new T();
            toAdd->Init(this);
            toAdd->viewport = dest->viewport;
            toAdd->viewport.w = dest->viewport.w / 2;
            if (!Insert(toAdd, dest, mode))
            {
                delete toAdd;
                toAdd = nullptr;
            }
            return toAdd;
        }

        /// Overload that just creates an editor window at the root of the window layout.
        template<typename T>
        typename enable_if<is_base_of<EditorWindow, T>::value, T*>::type
        Add(DockingMode mode)
        {
            T* toAdd = new T();
            toAdd->Init(this);
            EditorWindow* dest = nullptr;

            for (auto node : layout.GetRoots())
            {
                if (node->data.window != nullptr)
                {
                    dest = node->data.window;
                    break;
                }
                else
                {
                    // This is unlikely to happen, but just in case...
                    for (auto child : node->children)
                    {
                        if (child->data.window != nullptr)
                        {
                            // Just pick the first window that appears
                            // TODO: consider docking mode and pick appropriate window
                            dest = child->data.window;
                            break;
                        }
                    }
                }
            }

            if (dest != nullptr)
            {
                if (!Insert(toAdd, dest, mode))
                {
                    delete toAdd;
                    return nullptr;
                }
            }
            else
            {
                toAdd->renderer = renderer;
                toAdd->viewport = {0, 0, native->GetWidth(), native->GetHeight()};
                toAdd->node = layout.Insert(EditorRect(toAdd, toAdd->viewport));
            }
            return toAdd;
        }

        /// Marks the window to be removed at the end of the next Update call (or the current running Update() call).
        void RemovePostUpdate(EditorWindow* window);

        /// Returns the input controller instance this window is using.
        InputController* GetInputController();

        /// Returns the resource controller instance this window is using.
        ResourceController* GetResources();

        /// TODO: remove me
        Tree<EditorRect>* GetLayout();

    };

}

#endif // EDITORWINDOW_H
