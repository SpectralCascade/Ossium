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
#include "editorview.h"
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
        if (wasBordered)
        {
            viewport.x += 1;
            viewport.y += 1;
            viewport.w -= 2;
            viewport.h -= 2;
        }
        nativeOrigin = GetEditorLayout()->GetNativeWindow()->GetPosition();

        NeuronGUI::Refresh();

        if (wasBordered)
        {
            Vector2 previousMousePos = InputState.lastMousePos;
            Vector2 mousePos = InputState.mousePos;
            Vector2 mouseDownPos = InputState.mouseDownPos;

            viewport.x -= 1;
            viewport.y -= 1;
            viewport.w += 2;
            viewport.h += 2;

            // Reset renderer viewport to full window
            Vector2 windowDimensions = native->GetNativeWindow()->GetDimensions();
            Rect nativeRect = Rect(0, 0, windowDimensions.x, windowDimensions.y);
            renderer->SetViewportRect(nativeRect.SDL());

            // Here the window tabs, border etc. are drawn after doing the custom GUI bits
            Rect rect = Rect(viewport);
            rect.Draw(*renderer, borderColor);

            if (rect.Contains(mouseDownPos))
            {
                int resizePadding = 5;

                Rect left = Rect(rect.x, rect.y, resizePadding, rect.h);
                Rect top = Rect(rect.x, rect.y, rect.w, resizePadding);
                Rect right = Rect(rect.x + rect.w - resizePadding, rect.y, resizePadding, rect.h);
                Rect bottom = Rect(rect.x, rect.y + rect.h - resizePadding, rect.w, resizePadding);

                /*left.DrawFilled(*renderer, Colors::RED);
                top.DrawFilled(*renderer, Colors::RED);
                right.DrawFilled(*renderer, Colors::RED);
                bottom.DrawFilled(*renderer, Colors::RED);*/

                // Set the appropriate mouse cursor
                SDL_SystemCursor mouseCursor = MouseCursor::GetCurrentSystemCursor();
                if (top.Contains(mousePos))
                {
                    if (right.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    else if (left.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    else
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENS;
                    }
                }
                else if (bottom.Contains(mousePos))
                {
                    if (right.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    else if (left.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    else
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENS;
                    }
                }
                else if (left.Contains(mousePos))
                {
                    if (top.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    else if (bottom.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    else
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZEWE;
                    }
                }
                else if (right.Contains(mousePos))
                {
                    if (top.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                    }
                    else if (bottom.Contains(mousePos))
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                    }
                    else
                    {
                        mouseCursor = SDL_SYSTEM_CURSOR_SIZEWE;
                    }
                }
                else if (InputState.textFieldHovered)
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_IBEAM;
                }
                else
                {
                    mouseCursor = SDL_SYSTEM_CURSOR_ARROW;
                }

                if (MouseCursor::GetCurrentSystemCursor() != mouseCursor)
                {
                    MouseCursor::Set(mouseCursor);
                }
            }

        }

        if (bordered != wasBordered)
        {
            TriggerUpdate();
        }

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
        renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, native->GetWidth(), native->GetHeight());

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

    void EditorLayout::Update()
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
        if (updateViewports)
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
                        Logger::EngineLog().Debug("\nUpdating group [{0}] {1}", currentGroup, currentGroup->data);

                        bool currentRow = currentGroup->depth % 2 == layoutColumn;
                        float scaleFactor = (currentRow ? currentGroup->data.w : currentGroup->data.h) / (float)combinedSize;

                        Logger::EngineLog().Debug("Current group is a {0}, combined size = {1}, scale factor = {2}", currentRow ? "row" : "column", combinedSize, scaleFactor);

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
                if (native->IsMouseFocus())
                {
                    node->data.window->input->SetActive(true);
                }
                else
                {
                    node->data.window->input->SetActive(false);
                }
                node->data.window->Update(didUpdateViewports);

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
            Logger::EngineLog().Error("Cannot dock editor window to window that has a NULL layout node!");
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
                    source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::BOTTOM);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    dest->node = layout.Insert(destRect, dest->node);
                    source->node = layout.Insert(sourceRect, dest->node);
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
                    source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::RIGHT);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    dest->node = layout.Insert(destRect, dest->node);
                    source->node = layout.Insert(sourceRect, dest->node);
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
        //Logger::EngineLog().Debug("Source window viewport = {0}, dest window viewport = {1}", sourceRect, destRect);
        source->renderer = renderer;
        source->viewport = sourceRect.SDL();
        dest->viewport = destRect.SDL();
        source->native = this;

        updateViewports = true;
        return true;
    }

    bool EditorLayout::Remove(EditorWindow* source)
    {
        if (source->node->parent != nullptr && source->node->parent->children.size() == 2)
        {
            // When removing a node that only has one other sibling, the tree needs to be normalised
            // i.e. all nodes must have at least one sibling (except the root node), and every leaf must have a valid editor window instance.
            // There are 2 main cases in this scenario; one is that the sibling node is a leaf, and one where it is not.
            // First, get the sibling node.
            Node<EditorRect>* sibling = source->node->parent->children[(unsigned int)(source->node->parent->children[0] == source->node)];
            //Logger::EngineLog().Debug("Removing sibling {0}", sibling);

            if (sibling->data.window != nullptr)
            {
                // Simple case, sibling is a leaf so just move the sibling up a level
                sibling->parent->data.window = sibling->data.window;
                sibling->data.window->node = sibling->parent;
                layout.Remove(sibling);
                //Logger::EngineLog().Debug("Removed sibling which was a leaf, moved it up.");
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
                    //Logger::EngineLog().Debug("Removed sibling, made it root.");
                    // Flip the layout levels - rows become columns, columns become rows.
                    layoutRow = !layoutRow;
                    layoutColumn = !layoutColumn;
                }
                else
                {
                    // In this scenario, move all the children of the sibling node 2 levels up while maintaining the order.
                    int siblingParentIndex = -1;
                    for (auto child : sibling->parent->parent->children)
                    {
                        siblingParentIndex++;
                        if (child == sibling->parent)
                        {
                            break;
                        }
                    }
                    for (auto child : sibling->children)
                    {
                        child->SetParent(sibling->parent->parent, siblingParentIndex);
                        siblingParentIndex++;
                    }
                    // Remove both the sibling node and it's parent in one call
                    layout.Remove(sibling->parent);
                    //Logger::EngineLog().Debug("Removed sibling, moved it's children up 2 levels.");
                }
            }
            updateViewports = true;
        }
        //Logger::EngineLog().Debug("Removing node {0}", source->node);
        if (!layout.Remove(source->node))
        {
            return false;
        }
        if (source != nullptr)
        {
            delete source;
        }
        else
        {
            Logger::EngineLog().Warning("Attempted to remove editor window that is already destroyed!");
        }
        return true;
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
