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
#include "editorwindow.h"
#include "../../Core/mousecursor.h"

namespace Ossium::Editor
{

    //
    // EditorWindow
    //

    EditorWindow::~EditorWindow()
    {
        native->GetInputController()->RemoveContext(Utilities::Format("EditorWindow {0}", this));
        delete input;
        input = nullptr;
    }

    void EditorWindow::Init(EditorLayout* nativeWindow)
    {
        if (input == nullptr)
        {
            input = new InputContext();
        }
        if (nativeWindow != native)
        {
            nativeWindow->GetInputController()->AddContext(Utilities::Format("EditorWindow {0}", this), input);
            NeuronGUI::Init(input, nativeWindow->GetResources());
            native = nativeWindow;
        }
    }

    EditorLayout* EditorWindow::GetEditorLayout()
    {
        return native;
    }

    string EditorWindow::GetTitle()
    {
        return settings.title;
    }

    void EditorWindow::SetTitle(string title)
    {
        settings.title = title;
    }

    void EditorWindow::Destroy()
    {
        native->RemovePostUpdate(this);
    }

    void EditorWindow::Refresh()
    {
        bool wasBordered = bordered;

        renderer->SetViewportRect(viewport);

        // Clear just the viewport with the background color
        renderer->SetDrawColor(backgroundColor);
        SDL_Rect bgRect = {0, 0, viewport.w, viewport.h};
        SDL_RenderFillRect(renderer->GetRendererSDL(), &bgRect);

        if (wasBordered)
        {
            viewport.x += padding;
            viewport.y += padding;
            viewport.w -= padding * 2;
            viewport.h -= padding * 2;
        }
        nativeOrigin = GetEditorLayout()->GetNativeWindow()->GetPosition();

        NeuronGUI::Refresh();

        if (wasBordered)
        {
            Vector2 origin = Vector2(viewport.x, viewport.y);

            Vector2 mousePos = InputState.mousePos + origin;

            viewport.x -= padding;
            viewport.y -= padding;
            viewport.w += padding * 2;
            viewport.h += padding * 2;

            // Reset renderer viewport to full window
            Vector2 windowDimensions = native->GetNativeWindow()->GetDimensions();
            Rect nativeRect = Rect(0, 0, windowDimensions.x, windowDimensions.y);
            renderer->SetViewportRect(nativeRect.SDL());

            Rect rect = Rect(viewport);

            // Hitboxes for borders
            Rect left = Rect(rect.x, rect.y, padding, rect.h);
            Rect top = Rect(rect.x, rect.y, rect.w, padding);
            Rect right = Rect(rect.x + rect.w - padding, rect.y, padding, rect.h);
            Rect bottom = Rect(rect.x, rect.y + rect.h - padding, rect.w, padding);

            bool clicked = InputState.mousePressed && !InputState.mouseWasPressed;

            // Set the appropriate mouse cursor
            SDL_SystemCursor mouseCursor = MouseCursor::GetCurrentSystemCursor();
            if (top.Contains(mousePos))
            {
                if (right.Contains(mousePos))
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(1, -1);
                    }
                }
                else if (left.Contains(mousePos))
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(-1, -1);
                    }
                }
                else
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENS;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(0, -1);
                    }
                }
            }
            else if (bottom.Contains(mousePos))
            {
                if (right.Contains(mousePos))
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(1, 1);
                    }
                }
                else if (left.Contains(mousePos))
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(-1, 1);
                    }
                }
                else
                {
                    if (dragDir == Vector2::Zero)
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENS;
                    }
                    if (clicked)
                    {
                        dragDir = Vector2(0, 1);
                    }
                }
            }
            else if (left.Contains(mousePos))
            {
                if (dragDir == Vector2::Zero)
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_SIZEWE;
                }
                if (clicked)
                {
                    dragDir = Vector2(-1, 0);
                }
            }
            else if (right.Contains(mousePos))
            {
                if (dragDir == Vector2::Zero)
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_SIZEWE;
                }
                if (clicked)
                {
                    dragDir = Vector2(1, 0);
                }
            }
            else if (dragDir == Vector2::Zero && (rect.Contains(mousePos) || rect.Contains(InputState.lastMousePos + origin)) && !InputState.mousePressed)
            {
                if (InputState.textFieldHovered)
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_IBEAM;
                }
                else
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_ARROW;
                }
            }

            if (dragDir != Vector2::Zero)
            {
                // Now process user dragging
                if (InputState.mousePressed)
                {
                    switch ((int)dragDir.x)
                    {
                    case 1:
                        rect.w = mousePos.x - rect.x;
                        break;
                    case -1:
                        float xmax = rect.xmax();
                        rect.x = mousePos.x;
                        rect.w = xmax - rect.x;
                        break;
                    }

                    switch ((int)dragDir.y)
                    {
                    case 1:
                        rect.h = mousePos.y - rect.y;
                        break;
                    case -1:
                        float ymax = rect.ymax();
                        rect.y = mousePos.y;
                        rect.h = ymax - rect.y;
                        break;
                    }
                }

                // Resize the viewport in the layout
                GetEditorLayout()->Resize(this, rect);
            }

            if (MouseCursor::GetCurrentSystemCursor() != mouseCursor)
            {
                MouseCursor::Set(mouseCursor);
            }

            if (!InputState.mousePressed)
            {
                dragDir = Vector2::Zero;
            }

            // Draw the updated viewport
            Rect(viewport).Draw(*renderer, borderColor);
        }

        if (bordered != wasBordered)
        {
            TriggerUpdate();
        }

    }

    Node<EditorRect>* EditorWindow::GetLayoutNode()
    {
        return node;
    }

    //
    // EditorLayout
    //

    EditorLayout::EditorLayout(InputController* controller, ResourceController* resourceController, string title, int w, int h)
    {
        // Setup input and native window
        input = controller;
        resources = resourceController;
        input->AddContext(Utilities::Format("EditorLayout {0}", this), &windowContext);
        native = new Window(title.c_str(), w, h, false, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | (w < 0 || h < 0 ? SDL_WINDOW_MAXIMIZED : 0));

        // TODO: make this a method of the Window class
        SDL_SetWindowMinimumSize(native->GetWindowSDL(), (int)MIN_DIMENSIONS.x, (int)MIN_DIMENSIONS.y);

        renderer = new Renderer(native);
        renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w <= 0 ? 1 : w, h <= 0 ? 1 : h);

        // Use breadth-first traversal to get the flat tree
        layout.SetFlatTreeBuildMode(false);

        // Handle window resize
        native->OnSizeChanged += [&] (Window& window) {
            if (renderBuffer != NULL)
            {
                SDL_DestroyTexture(renderBuffer);
                renderBuffer = NULL;
            }
            renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window.GetWidth(), window.GetHeight());

            // Update the root node
            auto roots = layout.GetRoots();
            if (!roots.empty())
            {
                roots[0]->data.w = window.GetWidth();
                roots[0]->data.h = window.GetHeight();
            }

            updateViewports = true;

            return ActionOutcome::Ignore;
        };

        native->OnMouseEnter += [&] (Window& window) {
            for (auto node : layout.GetFlatTree()) {
                if (node->data.window != nullptr)
                {
                    node->data.window->input->SetActive(true);
                }
            }
        };

        native->OnMouseLeave += [&] (Window& window) {
            for (auto node : layout.GetFlatTree()) {
                if (node->data.window != nullptr)
                {
                    node->data.window->input->SetActive(false);
                }
            }
        };

    }

    EditorLayout::~EditorLayout()
    {
        if (renderBuffer != NULL)
        {
            SDL_DestroyTexture(renderBuffer);
            renderBuffer = NULL;
        }
        layout.Clear();
        if (renderer != nullptr)
        {
            delete renderer;
            renderer = nullptr;
        }
        if (native != nullptr)
        {
            /// NOTE TO FUTURE ME: this seems to set off the debugger?
            delete native;
            native = nullptr;
        }
    }

    void EditorLayout::HandleEvent(SDL_Event& e)
    {
        if (native != nullptr)
        {
            native->HandleEvent(e);
        }
    }

    Tree<EditorRect>* EditorLayout::GetLayout()
    {
        return &layout;
    }

    void EditorLayout::Update(bool forceUpdate)
    {

        // Setup rendering to texture
        SDL_Renderer* render = renderer->GetRendererSDL();
        SDL_SetRenderTarget(render, renderBuffer);

        // These are used for computing viewport sizes
        Node<EditorRect>* currentGroup = nullptr;
        int combinedSize = 0;

        // Breadth-first layout iteration; just in case the flat tree is updated during this process (e.g. another editor window is added), only iterate up to the current end point.
        vector<Node<EditorRect>*>& flatTree = layout.GetFlatTree();
        auto endItr = flatTree.end();
        auto length = flatTree.size();

        // First stage - update the viewports (but only if necessary).
        // Optimiser: keep track of nodes that need updating and only update those sections of the layout tree?
        if (updateViewports || forceUpdate)
        {
            for (auto itr = flatTree.begin(); itr < endItr; itr++)
            {
                Node<EditorRect>* node = *itr;

                if (node->parent != nullptr)
                {
                    bool isRow = node->depth % 2 == layoutRow;

                    // Have we just iterated over a grouping of nodes (or the very last node)?
                    bool lastNode = itr == flatTree.begin() + (length - 1);
                    if ((currentGroup != node->parent || lastNode) && currentGroup != nullptr)
                    {
                        if (lastNode && currentGroup == node->parent)
                        {
                            combinedSize += isRow ? node->data.w : node->data.h;
                        }
                        //Log.Debug("\nUpdating group [{0}] {1}", currentGroup, currentGroup->data);

                        bool currentRow = currentGroup->depth % 2 == layoutColumn;
                        float scaleFactor = (currentRow ? currentGroup->data.w : currentGroup->data.h) / (float)combinedSize;

                        //Log.Debug("Current group is a {0}, combined size = {1}, scale factor = {2}", currentRow ? "row" : "column", combinedSize, scaleFactor);

                        // Update the entire group of rectangles now we know their total combined width/height
                        if (currentRow)
                        {
                            float nextPos = currentGroup->data.x;
                            for (Node<EditorRect>* child : currentGroup->children)
                            {
                                float change = (float)child->data.w * scaleFactor;
                                child->data.w = round(change);
                                child->data.x = round(nextPos);
                                child->data.y = currentGroup->data.y;
                                child->data.h = currentGroup->data.h;
                                if (child->data.window != nullptr)
                                {
                                    child->data.window->viewport = child->data.SDL();
                                }
                                nextPos += change;
                            }
                        }
                        else
                        {
                            float nextPos = currentGroup->data.y;
                            for (Node<EditorRect>* child : currentGroup->children)
                            {
                                float change = (float)child->data.h * scaleFactor;
                                child->data.h = round(change);
                                child->data.y = round(nextPos);
                                child->data.x = currentGroup->data.x;
                                child->data.w = currentGroup->data.w;
                                if (child->data.window != nullptr)
                                {
                                    child->data.window->viewport = child->data.SDL();
                                }
                                nextPos += change;
                            }
                        }

                        combinedSize = isRow ? node->data.w : node->data.h;
                    }
                    else
                    {
                        combinedSize += isRow ? node->data.w : node->data.h;
                    }
                }
                else
                {
                    // Must be a root node (for this use case, there will only be one root node).
                    // Always matches the native window rect.
                    node->data.w = native->GetWidth();
                    node->data.h = native->GetHeight();
                    node->data.x = 0;
                    node->data.y = 0;
                }
                currentGroup = node->parent;

                if (node->data.window != nullptr)
                {
                    node->data.window->viewport = node->data.SDL();
                }
            }
        }
        bool didUpdateViewports = updateViewports;
        updateViewports = false;

        // Second stage - logic and rendering.
        for (auto itr = flatTree.begin(); itr != endItr; itr++)
        {
            Node<EditorRect>* node = *itr;

            // Now update the editor window, if this node is not a stem
            if (node->data.window != nullptr)
            {
                node->data.window->Update(didUpdateViewports || forceUpdate);
            }
        }

        renderer->SetViewportRect({0, 0, native->GetWidth(), native->GetHeight()});

        // Change target back to the window and render the texture
        SDL_SetRenderTarget(render, NULL);
        SDL_RenderClear(render);
        SDL_RenderCopy(render, renderBuffer, NULL, NULL);
        SDL_RenderPresent(renderer->GetRendererSDL());

        // Only remove windows when everything has finished updating.
        if (!toRemove.empty())
        {
            updateViewports = true;
            for (auto window : toRemove)
            {
                Remove(window);
            }
            toRemove.clear();
        }
    }

    Window* EditorLayout::GetNativeWindow()
    {
        return native;
    }

    bool EditorLayout::Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode)
    {
        if (dest->node == nullptr)
        {
            Log.Error("Cannot dock editor window to window that has a NULL layout node!");
            return false;
        }

        EditorRect sourceRect = EditorRect(source->viewport);
        sourceRect.window = source;
        EditorRect destRect = dest->node->data;
        switch (mode)
        {
            case TOP:
            case BOTTOM:
                if (dest->node->depth == 0)
                {
                    // Make the current destination root
                    dest->node->data.window = nullptr;
                    // Move the destination node down a level
                    dest->node = layout.Insert(destRect, dest->node);
                    destRect.window->node = dest->node;
                    layoutRow = 0;
                    layoutColumn = 1;
                }

                // TODO: proper dimension checks, return false if there's not enough space
                destRect.h = max(MIN_DIMENSIONS.y, destRect.h - sourceRect.h);

                // Warning: rect positioning and resizing assumes the source rect has already been resized to fit!
                if (mode == DockingMode::BOTTOM)
                {
                    sourceRect.y = destRect.y + destRect.h;
                }
                else
                {
                    sourceRect.y = destRect.y;
                    destRect.y += sourceRect.h;
                }
                sourceRect.w = destRect.w;
                sourceRect.x = destRect.x;

                if (dest->node->depth % 2 == layoutColumn)
                {
                    source->node = layout.Insert(sourceRect, dest->node->parent, mode == DockingMode::BOTTOM ? dest->node->childIndex + 1 : dest->node->childIndex);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    auto parent = dest->node;
                    dest->node = layout.Insert(destRect, parent);
                    source->node = layout.Insert(sourceRect, parent, mode == DockingMode::BOTTOM);
                }
                break;
            case LEFT:
            case RIGHT:
                // Same as above indent block, but for inserting as a column rather than as a row.
                if (dest->node->depth == 0)
                {
                    // Make the current destination root
                    dest->node->data.window = nullptr;
                    // Move the destination node down a level
                    dest->node = layout.Insert(destRect, dest->node);
                    destRect.window->node = dest->node;
                    layoutRow = 1;
                    layoutColumn = 0;
                }

                // TODO: proper dimension checks, return false if there's not enough space
                destRect.w = max(MIN_DIMENSIONS.x, destRect.w - sourceRect.w);

                // Warning: rect positioning and resizing assumes the source rect has already been resized to fit!
                if (mode == DockingMode::RIGHT)
                {
                    sourceRect.x = destRect.x + destRect.w;
                }
                else
                {
                    sourceRect.x = destRect.x;
                    destRect.x += sourceRect.w;
                }
                sourceRect.h = destRect.h;
                sourceRect.y = destRect.y;

                if (dest->node->depth % 2 == layoutRow)
                {
                    source->node = layout.Insert(sourceRect, dest->node->parent, mode == DockingMode::RIGHT ? dest->node->childIndex + 1 : dest->node->childIndex);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    auto parent = dest->node;
                    dest->node = layout.Insert(destRect, parent);
                    source->node = layout.Insert(sourceRect, parent, mode == DockingMode::RIGHT);
                }

                break;
            case TOP_ADJACENT:
            case BOTTOM_ADJACENT:
                break;
            case LEFT_ADJACENT:
            case RIGHT_ADJACENT:
                break;
            default:
                // Invalid docking mode
                break;
        }
        dest->node->data = destRect;
        source->node->data = sourceRect;
        //Log.Debug("Source window viewport = {0}, dest window viewport = {1}", sourceRect, destRect);
        source->renderer = renderer;
        source->viewport = sourceRect.SDL();
        dest->viewport = destRect.SDL();
        source->native = this;

        updateViewports = true;
        return true;
    }

    bool EditorLayout::Remove(EditorWindow* source)
    {
        // First check for a single sibling node
        Node<EditorRect>* sibling = nullptr;
        if (source != nullptr && source->node != nullptr && source->node->parent != nullptr && source->node->parent->children.size() == 2)
        {
            sibling = source->node->parent->children[(unsigned int)(!source->node->childIndex)];
        }
        // Now the sibling is determined, the source node can be safely removed
        if (!layout.Remove(source->node))
        {
            Log.Error("Failed to remove EditorWindow node!");
        }

        if (sibling != nullptr)
        {
            // When removing a node that only has one other sibling, the tree needs to be normalised
            // i.e. all nodes must have at least one sibling (except the root node), and every leaf must have a valid editor window instance.
            // There are 2 main cases in this scenario; one is that the sibling node is a leaf, and one where it is not.

            if (sibling->data.window != nullptr)
            {
                // Simple case, sibling is a leaf so just move the sibling up a level
                sibling->parent->data.window = sibling->data.window;
                sibling->data.window->node = sibling->parent;
                layout.Remove(sibling);
                //Log.Debug("Removed sibling which was a leaf, moved it up.");
            }
            else
            {
                // Complex case, ditch the sibling but keep it's children while maintaining tree normalisation.
                if (sibling->depth == 1)
                {
                    // If the parent is the root node, then the root node becomes the sibling and the layout levels are switched.
                    sibling->parent->data.window = sibling->data.window;
                    for (auto child : sibling->children)
                    {
                        child->SetParent(sibling->parent);
                    }
                    layout.Remove(sibling);
                    //Log.Debug("Removed sibling, made it root.");
                    // Flip the layout levels - rows become columns, columns become rows.
                    layoutRow = !layoutRow;
                    layoutColumn = !layoutColumn;
                }
                else
                {
                    // In this scenario, move all the children of the sibling node 2 levels up while maintaining the order.
                    int insertIndex = sibling->parent->childIndex;
                    //Log.Debug("Removing null sibling at depth {0} child index {1}, moved it's children up 2 levels.", sibling->depth, insertIndex);
                    vector<Node<EditorRect>*> children = sibling->children;
                    for (auto child : children)
                    {
                        child->SetParent(sibling->parent->parent, insertIndex);
                        //Log.Info("Inserted child at {0} (new depth = {1}, true child index = {2})", insertIndex, child->depth, child->childIndex);
                        insertIndex++;
                    }
                    // Remove both the sibling node and it's parent
                    layout.Remove(sibling->parent);
                }
            }
            updateViewports = true;
            //Log.Debug("Normalised tree after removing a window node");
        }
        if (source != nullptr)
        {
            Log.Debug("Destroying window \"{0}\"", source->title);
            delete source;
        }
        else
        {
            Log.Warning("Attempted to remove editor window that is already destroyed!");
        }
        return true;
    }

    void EditorLayout::Resize(EditorWindow* window, Rect rect)
    {
        // First, normalise the rect
        rect = Rect(Utilities::Clamp(rect.x, 0.0f, (float)native->GetWidth()),
                    Utilities::Clamp(rect.y, 0.0f, (float)native->GetHeight()),
                    Utilities::Clamp(rect.w, MIN_DIMENSIONS.x, (float)native->GetWidth()),
                    Utilities::Clamp(rect.h, MIN_DIMENSIONS.y, (float)native->GetHeight())
        );

        Node<EditorRect>* node = window->node;
        Node<EditorRect>* parent = node->parent;

        // Locate siblings
        Node<EditorRect>* previous = parent != nullptr && node->childIndex > 0 ? parent->children[node->childIndex - 1] : nullptr;
        Node<EditorRect>* next = parent != nullptr && parent->children.size() > node->childIndex + 1 ? parent->children[node->childIndex + 1] : nullptr;

        // Locate guncles
        if (parent != nullptr && parent->parent != nullptr)
        {
            if (previous == nullptr)
            {
                previous = parent->childIndex > 0 ? parent->parent->children[parent->childIndex - 1] : nullptr;
            }
            if (next == nullptr)
            {
                next = parent->parent->children.size() > parent->childIndex + 1 ? parent->parent->children[parent->childIndex + 1] : nullptr;
            }
        }

        float minpos = 0, maxpos = 0;

        // Now the fun begins
        if (rect.x != node->data.x)
        {
            maxpos = node->data.xmax() - MIN_DIMENSIONS.x;
            if (previous != nullptr)
            {
                minpos = previous->data.x + MIN_DIMENSIONS.x;
            }
            else
            {
                minpos = MIN_DIMENSIONS.x;
            }
            float xmax = node->data.xmax();
            node->data.x = Utilities::Clamp(rect.x, minpos, maxpos);
            node->data.w = xmax - node->data.x;
            if (previous != nullptr)
            {
                previous->data.w = node->data.x - previous->data.x;
            }
            updateViewports = true;
        }
        else if (rect.w != node->data.w)
        {
            minpos = MIN_DIMENSIONS.x;
            if (next != nullptr)
            {
                maxpos = (next->data.xmax() - node->data.x) - MIN_DIMENSIONS.x;
            }
            else
            {
                maxpos = native->GetWidth() - node->data.x;
            }
            node->data.w = Utilities::Clamp(rect.w, minpos, maxpos);
            if (next != nullptr)
            {
                next->data.w = next->data.xmax() - node->data.xmax();
                next->data.x = node->data.xmax();
            }
            updateViewports = true;
        }

        if (rect.y != node->data.y)
        {
            maxpos = node->data.ymax() - MIN_DIMENSIONS.y;
            if (previous != nullptr)
            {
                minpos = previous->data.y + MIN_DIMENSIONS.y;
            }
            else
            {
                minpos = MIN_DIMENSIONS.y;
            }
            float ymax = node->data.ymax();
            node->data.y = Utilities::Clamp(rect.y, minpos, maxpos);
            node->data.h = ymax - node->data.y;
            if (previous != nullptr)
            {
                previous->data.h = node->data.y - previous->data.y;
            }
            updateViewports = true;
        }
        else if (rect.h != node->data.h)
        {
            minpos = MIN_DIMENSIONS.y;
            if (next != nullptr)
            {
                maxpos = (next->data.ymax() - node->data.y) - MIN_DIMENSIONS.y;
            }
            else
            {
                maxpos = native->GetHeight() - node->data.y;
            }
            node->data.h = Utilities::Clamp(rect.h, minpos, maxpos);
            if (next != nullptr)
            {
                next->data.h = next->data.ymax() - node->data.ymax();
                next->data.y = node->data.ymax();
            }
            updateViewports = true;
        }

    }

    void EditorLayout::RemovePostUpdate(EditorWindow* window)
    {
        toRemove.insert(window);
    }

    InputController* EditorLayout::GetInputController()
    {
        return input;
    }

    ResourceController* EditorLayout::GetResources()
    {
        return resources;
    }

}
