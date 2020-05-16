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
        /*native->OnSizeChanged += [&] (Window& window) {
            if (renderBuffer != NULL)
            {
                SDL_DestroyTexture(renderBuffer);
                renderBuffer = NULL;
            }
            renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window.GetWidth(), window.GetHeight());
            updateViewports = true;

            return ActionOutcome::Ignore;
        };*/

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

    void NativeEditorWindow::Update()
    {

        // Setup rendering to texture
        SDL_Renderer* render = renderer->GetRendererSDL();
        SDL_SetRenderTarget(render, renderBuffer);

        // These are used for computing viewport sizes
        Node<EditorRect>* currentGroup = nullptr;
        int combinedSize = 0;

        // Breadth-first layout iteration
        vector<Node<EditorRect>*>& flatTree = layout.GetFlatTree();
        for (vector<Node<EditorRect>*>::iterator itr = flatTree.begin(); itr != flatTree.end(); itr++)
        {
            Node<EditorRect>* node = *itr;
/*
            // First, update the viewports
            if (updateViewports)
            {
                if (node->parent != nullptr)
                {
                    bool isRow = node->depth % 2 == (int)layoutRow;

                    // Have we just iterated over a grouping of nodes (or the very last node)?
                    if ((currentGroup != node->parent || itr == flatTree.begin() + (flatTree.size() - 1)) && currentGroup != nullptr)
                    {
                        bool currentRow = currentGroup->depth % 2 == layoutColumn;
                        float scaleFactor = (float)(currentRow ? currentGroup->data.w : currentGroup->data.h) / (float)combinedSize;

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
            }*/

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
        updateViewports = false;

        renderer->SetViewportRect({0, 0, native->GetWidth(), native->GetHeight()});

        // Change target back to the window and render the texture
        SDL_SetRenderTarget(render, NULL);
        SDL_RenderClear(render);
        SDL_RenderCopy(render, renderBuffer, NULL, NULL);
        SDL_RenderPresent(renderer->GetRendererSDL());

        for (auto window : toRemove)
        {
            Remove(window);
        }
        toRemove.clear();
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
                    rect.window = nullptr;
                    dest->node->SetParent(layout.Insert(rect, nullptr));
                    layoutRow = 1;
                    layoutColumn = 0;
                }
                else if (dest->node->depth % 2 == layoutColumn) // Alternating depths correspond to alternating row or column layout direction.
                {
                    // In this scenario, rearrange the position of the destination node in the layout tree so it's at the correct depth.
                    EditorRect rect = dest->node->data;
                    rect.window = nullptr;
                    dest->node->SetParent(layout.Insert(rect, dest->node, false));
                }
                source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::BOTTOM);
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
                    rect.window = nullptr;
                    dest->node->SetParent(layout.Insert(rect, nullptr));
                    layoutRow = 0;
                    layoutColumn = 1;
                }
                if (dest->node->depth % 2 == layoutRow)
                {
                    EditorRect rect = dest->node->data;
                    rect.window = nullptr;
                    dest->node->SetParent(layout.Insert(rect, dest->node, false));
                }
                source->node = layout.Insert(sourceRect, dest->node, mode == DockingMode::RIGHT);

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
        Logger::EngineLog().Info("Source window viewport = {0}, dest window viewport = {1}", sourceRect, destRect);
        source->renderer = renderer;
        source->viewport = sourceRect.SDL();
        dest->viewport = destRect.SDL();
        source->native = this;

        return true;
    }

    /*void NativeEditorWindow::ResizeGroup(vector<Node<EditorWindow*>*>& group, Rect parentRect)
    {
        // TODO
        Vector2 position = Vector2(parentRect.x, parentRect.y);

        for (auto node : group)
        {

            if (node->data != nullptr)
            {
            }
            else
            {
            }
        }
    }*/

    bool NativeEditorWindow::Remove(EditorWindow* source)
    {
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
            Logger::EngineLog().Warning("Attempting to remove editor window that is already destroyed?");
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
