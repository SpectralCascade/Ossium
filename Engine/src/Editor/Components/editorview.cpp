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
        inputController->RemoveContext(Utilities::Format("EditorWindow {0}", this));
        delete input;
        input = nullptr;
    }

    void EditorWindow::Init(InputController* inputControl, ResourceController* resourceController, NativeEditorWindow* nativeWindow)
    {
        input = new InputContext();
        inputController = inputControl;
        inputController->AddContext(Utilities::Format("EditorWindow {0}", this), input);
        NeuronGUI::Init(input, resourceController);
        native = nativeWindow;
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

    void EditorWindow::Close()
    {
        native->Remove(this);
    }

    NativeEditorWindow::NativeEditorWindow(InputController* controller, int w, int h)
    {
        // Setup input and native window
        input = controller;
        windowContext = new InputContext();
        input->AddContext(Utilities::Format("NativeEditorWindow {0}", this), windowContext);
        native = windowContext->GetHandler<Window>();
        native->Init("Ossium (Editor)", w, h, false, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        // TODO: make this a method of the Window class
        SDL_SetWindowMinimumSize(native->GetWindowSDL(), (int)MIN_DIMENSIONS.x, (int)MIN_DIMENSIONS.y);

        renderer = new Renderer(native);
        renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, native->GetWidth(), native->GetHeight());

        // Handle window resize
        native->AddAction(
            "ResizeTexture",
            [&] (const WindowInput& window) {
                if (renderBuffer != NULL)
                {
                    SDL_DestroyTexture(renderBuffer);
                    renderBuffer = NULL;
                }
                renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window.raw.data1, window.raw.data2);
                // Redraw all editor windows on next update because the texture was destroyed
                for (auto node : layout.GetFlatTree())
                {
                    if (node->data != nullptr)
                    {
                        node->data->TriggerUpdate();
                    }
                }
                return ActionOutcome::Ignore;
            },
            SDL_WINDOWEVENT_SIZE_CHANGED
        );

    }

    NativeEditorWindow::~NativeEditorWindow()
    {
        if (renderBuffer != NULL)
        {
            SDL_DestroyTexture(renderBuffer);
            renderBuffer = NULL;
        }
        layout.Clear();
        input->RemoveContext(Utilities::Format("NativeEditorWindow {0}", this));
        delete windowContext;
        delete renderer;
    }

    void NativeEditorWindow::Update()
    {
        // Setup rendering to texture
        SDL_Renderer* render = renderer->GetRendererSDL();
        SDL_SetRenderTarget(render, renderBuffer);

        for (auto node : layout.GetFlatTree())
        {
            if (node->data != nullptr)
            {
                if (native->IsMouseFocus())
                {
                    node->data->input->SetActive(true);
                }
                else
                {
                    node->data->input->SetActive(false);
                }
                node->data->Update();
            }
        }
        renderer->SetViewportRect({0, 0, native->GetWidth(), native->GetHeight()});

        // Change target back to the window and render the texture
        SDL_SetRenderTarget(render, NULL);
        SDL_RenderClear(render);
        SDL_RenderCopy(render, renderBuffer, NULL, NULL);
        SDL_RenderPresent(renderer->GetRendererSDL());
    }

    bool NativeEditorWindow::Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode)
    {
        if (dest->node == nullptr)
        {
            Logger::EngineLog().Error("Cannot dock editor window to window that has a NULL layout node!");
            return false;
        }

        SDL_Rect viewport = source->viewport;
        SDL_Rect destViewport = dest->viewport;
        if (mode & (TOP | BOTTOM))
        {
            if (dest->node->depth == 0)
            {
                // If the chosen editor window is the root, move it down to depth 1 first as root should be null if there is more than one child.
                dest->node->SetParent(layout.Insert(nullptr, nullptr));
                layoutRow = 1;
                layoutColumn = 0;
            }
            // Alternating depths correspond to alternating row or column layout direction.
            if (dest->node->depth % 2 == layoutColumn)
            {
                // In this scenario, rearrange the position of the destination node in the layout tree so it's at the correct depth.
                dest->node->SetParent(layout.Insert(nullptr, dest->node, false));
            }
            source->node = layout.Insert(source, dest->node, mode == DockingMode::BOTTOM);
            // Now set the dimensions of the viewports.
            viewport.w = destViewport.w;
            if (viewport.h == 0)
            {
                // TODO: calculate height as: 2 + (total dest siblings) / window height
                viewport.h = destViewport.h / 2;
            }
            destViewport.h = destViewport.h - viewport.h;
            if (destViewport.h < MIN_DIMENSIONS.y)
            {
                // TODO: handle cases where the inserted window is too big
            }
            // Now set the position of the source viewport.
            viewport.x = destViewport.x;
            viewport.y = destViewport.y + (mode == DockingMode::BOTTOM ? destViewport.h : -destViewport.h);
        }
        else
        {
            // Same as above indent block, but for inserting as a column rather than as a row.
            if (dest->node->depth == 0)
            {
                dest->node->SetParent(layout.Insert(nullptr, nullptr));
                layoutRow = 0;
                layoutColumn = 1;
            }
            if (dest->node->depth % 2 == layoutRow)
            {
                dest->node->SetParent(layout.Insert(nullptr, dest->node, false));
            }
            source->node = layout.Insert(source, dest->node, mode == DockingMode::RIGHT);

            viewport.h = destViewport.h;
            if (viewport.w == 0)
            {
                // TODO: see height TODO, do equivalent for width here.
                viewport.w = destViewport.w / 2;
            }
            destViewport.w = destViewport.w - viewport.w;
            if (destViewport.h < MIN_DIMENSIONS.y)
            {
                // TODO: handle cases where the inserted window is too big
            }
            viewport.x = destViewport.x + (mode == DockingMode::RIGHT ? destViewport.w : -destViewport.w);
            viewport.y = destViewport.y;
        }
        Logger::EngineLog().Info("Source window viewport = {0}, dest window viewport = {1}", viewport, destViewport);
        source->renderer = renderer;
        source->viewport = viewport;
        dest->viewport = destViewport;
        source->native = this;

        return true;
    }

    void NativeEditorWindow::UpdateViewports()
    {
        // TODO
    }

    void NativeEditorWindow::Remove(EditorWindow* source)
    {
        // TODO: !!!! DON'T REMOVE CHILD NODES !!!! also resize sibling and child editor windows
        layout.Remove(source->node);
        delete source;
    }

}
