#ifndef EDITORLAYOUT_H
#define EDITORLAYOUT_H

#include "../../Ossium.h"

#include "editorwindow.h"
#include "../../Core/tree.h"

namespace Ossium::Editor
{

    class EditorWindow;
    class EditorController;

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

    /// This holds the actual native editor window and the layout tree for docked editor windows.
    class EditorLayout
    {
    protected:
        /// The renderer to use for this window.
        Renderer* renderer = nullptr;

        /// The input context for the native window.
        InputContext windowContext;

        /// The input controller instance.
        InputController* input;

        /// The resource controller instance.
        ResourceController* resources;

        /// Minimum dimensions of the native window.
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
        std::set<EditorWindow*> toRemove;

        /// Should the viewports be updated on the next Update() call?
        bool updateViewports = false;

        /// Reference back to the editor controller instance.
        EditorController* controller = nullptr;

        /// Docks an existing source editor window to a destination editor window.
        bool Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode);

        /// Removes an editor window from this native editor window.
        bool Remove(EditorWindow* source);

        // Recursive search up the tree for an adjacent node. Returns nullptr if no node is found.
        Node<EditorRect>* FindSibling(Node<EditorRect>* node, int dir, bool isRow);

        /// Services available to editor windows in this layout, such as the renderer and resource controller.
        ServicesProvider* services = nullptr;

    public:
        /// Creates the window and initialises the tree.
        EditorLayout(EditorController* controller, std::string title = "Untitled", int w = -1, int h = -1);
        virtual ~EditorLayout();

        /// Handles an incoming SDL_Event.
        void HandleEvent(SDL_Event& e);

        /// Update editor windows.
        void Update(bool forceUpdate = false);

        /// Return the native OS window instance.
        Window* GetNativeWindow();

        /// Obtain a particular service.
        template<typename T>
        typename std::enable_if<std::is_base_of<ServiceBase, T>::value, T*>::type
        GetService()
        {
            return services->GetService<T>();
        }

        /// Returns a pointer to the services provider.
        ServicesProvider* GetServices();

        /// Attempts to resize and editor window rect given an input rect.
        void Resize(EditorWindow* window, Rect rect);

        /// Instantiates a specific editor window instance and initialises it, then inserts within the layout tree.
        template<typename T>
        typename std::enable_if<std::is_base_of<EditorWindow, T>::value, T*>::type
        Add(EditorWindow* dest, DockingMode mode)
        {
            T* toAdd = new T();
            toAdd->Init(this);
            toAdd->viewport = dest->viewport;
            toAdd->viewport.w = dest->viewport.w / 2;
            toAdd->viewport.h = dest->viewport.h / 2;
            if (!Insert(toAdd, dest, mode))
            {
                delete toAdd;
                toAdd = nullptr;
            }
            return toAdd;
        }

        /// Overload that just creates an editor window at the root of the window layout.
        template<typename T>
        typename std::enable_if<std::is_base_of<EditorWindow, T>::value, T*>::type
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

        /// Returns the main editor controller.
        EditorController* GetEditorController();

    };

}

#endif // EDITORLAYOUT_H
