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

    EditorWindow::~EditorWindow()
    {
        native->GetInputController()->RemoveContext(Utilities::Format("EditorWindow {0}", this));
        delete input;
        input = nullptr;
    }

    void EditorWindow::Init(NativeEditorWindow* nativeWindow)
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

    NativeEditorWindow* EditorWindow::GetNativeWindow()
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

    NativeEditorWindow::NativeEditorWindow(InputController* controller, ResourceController* resourceController, string title, int w, int h)
    {
        // Setup input and native window
        input = controller;
        resources = resourceController;
        input->AddContext(Utilities::Format("NativeEditorWindow {0}", this), &windowContext);
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
            updateViewports = true;

            return ActionOutcome::Ignore;
        };

    }

    NativeEditorWindow::~NativeEditorWindow()
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

    void NativeEditorWindow::HandleEvent(SDL_Event& e)
    {
        if (native != nullptr)
        {
            native->HandleEvent(e);
        }
    }

    Tree<EditorRect>* NativeEditorWindow::GetLayout()
    {
        return &layout;
    }

    void NativeEditorWindow::Update()
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
            for (auto itr = flatTree.begin(); itr != endItr; itr++)
            {
                Node<EditorRect>* node = *itr;

                if (node->parent != nullptr)
                {
                    bool isRow = node->depth % 2 == (int)layoutRow;

                    // Have we just iterated over a grouping of nodes (or the very last node)?
                    if ((currentGroup != node->parent || itr == flatTree.begin() + (length - 1)) && currentGroup != nullptr)
                    {
                        bool currentRow = currentGroup->depth % 2 == layoutColumn;
                        float scaleFactor = (float)(currentRow ? currentGroup->data.w : currentGroup->data.h) / (float)combinedSize;

                        // Update the entire group of rectangles now we know their total combined width/height
                        if (currentRow)
                        {
                            if (combinedSize != currentGroup->data.w)
                            {
                                float nextPos = currentGroup->data.x;
                                for (Node<EditorRect>* child : currentGroup->children)
                                {
                                    float change = (float)child->data.w * scaleFactor;
                                    child->data.w = round(change);
                                    child->data.x = round(nextPos);
                                    child->data.y = currentGroup->data.y;
                                    child->data.h = currentGroup->data.h;
                                    nextPos += change;
                                }
                            }
                        }
                        else
                        {
                            if (combinedSize != currentGroup->data.h)
                            {
                                float nextPos = currentGroup->data.y;
                                for (Node<EditorRect>* child : currentGroup->children)
                                {
                                    float change = (float)child->data.h * scaleFactor;
                                    child->data.h = round(change);
                                    child->data.y = round(nextPos);
                                    child->data.x = currentGroup->data.x;
                                    child->data.w = currentGroup->data.w;
                                    nextPos += change;
                                }
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
            updateViewports = false;
        }

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
                node->data.window->Update();

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

    bool NativeEditorWindow::Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode)
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
                    // If the chosen editor window is the root, move it down to depth 1 first as root should be null if there is more than one child.
                    EditorRect rect = dest->node->data;
                    dest->node = layout.Insert(rect, dest->node);
                    layoutRow = 1;
                    layoutColumn = 0;
                }
                if (dest->node->depth % 2 == layoutColumn) // Alternating depths correspond to alternating row or column layout direction.
                {
                    ///
                    /// TODO: handle case where the destination node's sibling is _not_ a leaf node.
                    ///

                    // In this scenario, rearrange the position of the destination node in the layout tree so it's at the correct depth.
                    EditorRect rect = dest->node->data;
                    dest->node->data.window = nullptr;
                    dest->node = layout.Insert(rect, dest->node);
                    source->node = layout.Insert(sourceRect, dest->node);
                }
                else
                {
                    source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::BOTTOM);
                }

                // Now set the dimensions of the viewports.
                sourceRect.w = destRect.w;
                if (sourceRect.h == 0)
                {
                    // TODO: calculate height as: 2 + (total dest siblings) / window height
                    sourceRect.h = destRect.h / 2;
                }
                destRect.h = destRect.h - sourceRect.h;
                if (destRect.h < MIN_DIMENSIONS.y)
                {
                    // TODO: handle cases where the inserted window is too big
                }
                // Now set the position of the source viewport.
                sourceRect.x = destRect.x;
                sourceRect.y = destRect.y + (mode == DockingMode::BOTTOM ? destRect.h : -destRect.h);
                break;
            case LEFT:
            case RIGHT:
                // Same as above indent block, but for inserting as a column rather than as a row.
                if (dest->node->depth == 0)
                {
                    EditorRect rect = dest->node->data;
                    // Make it root
                    dest->node->data.window = nullptr;
                    dest->node = layout.Insert(rect, dest->node);
                    layoutRow = 0;
                    layoutColumn = 1;
                }
                if (dest->node->depth % 2 == layoutRow)
                {
                    EditorRect rect = dest->node->data;
                    // Make the destination node a sibling instead of a parent
                    dest->node->data.window = nullptr;
                    dest->node = layout.Insert(rect, dest->node);
                    source->node = layout.Insert(sourceRect, dest->node);
                }
                else
                {
                    source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::RIGHT);
                }

                sourceRect.h = destRect.h;
                if (sourceRect.w == 0)
                {
                    // TODO: see height TODO, do equivalent for width here.
                    sourceRect.w = destRect.w / 2;
                }
                destRect.w = destRect.w - sourceRect.w;
                if (destRect.h < MIN_DIMENSIONS.y)
                {
                    // TODO: handle cases where the inserted window is too big
                }
                sourceRect.x = destRect.x + (mode == DockingMode::RIGHT ? destRect.w : -destRect.w);
                sourceRect.y = destRect.y;
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
        Logger::EngineLog().Debug("Source window viewport = {0}, dest window viewport = {1}", sourceRect, destRect);
        source->renderer = renderer;
        source->viewport = sourceRect.SDL();
        dest->viewport = destRect.SDL();
        source->native = this;

        updateViewports = true;
        return true;
    }

    bool NativeEditorWindow::Remove(EditorWindow* source)
    {
        if (source->node->parent != nullptr && source->node->parent->children.size() == 2)
        {
            // When removing a node that only has one other sibling, the tree needs to be normalised
            // i.e. all nodes must have at least one sibling (except the root node), and every leaf must have a valid editor window instance.
            // There are 2 main cases in this scenario; one is that the sibling node is a leaf, and one where it is not.
            // First, get the sibling node.
            Node<EditorRect>* sibling = source->node->parent->children[(unsigned int)(source->node->parent->children[0] == source->node)];
            Logger::EngineLog().Debug("Removing sibling {0}", sibling);

            if (sibling->data.window != nullptr)
            {
                // Simple case, sibling is a leaf so just move the sibling up a level
                sibling->parent->data.window = sibling->data.window;
                sibling->data.window->node = sibling->parent;
                if (!layout.Remove(sibling))
                {
                    Logger::EngineLog().Error("Failed to remove sibling!");
                }
                Logger::EngineLog().Debug("Removed sibling which was a leaf, moved it up.");
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
                    Logger::EngineLog().Debug("Removed sibling, made it root.");
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
                    Logger::EngineLog().Debug("Removed sibling, moved it's children up 2 levels.");
                }
            }
            updateViewports = true;
        }
        Logger::EngineLog().Debug("Removing node {0}", source->node);
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

    void NativeEditorWindow::RemovePostUpdate(EditorWindow* window)
    {
        toRemove.insert(window);
    }

    InputController* NativeEditorWindow::GetInputController()
    {
        return input;
    }

    ResourceController* NativeEditorWindow::GetResources()
    {
        return resources;
    }

}
