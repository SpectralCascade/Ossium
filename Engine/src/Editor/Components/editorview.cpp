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
        OnDestroy(*this);
        inputController->RemoveContext(Utilities::Format("EditorWindow {0}", input));
        delete input;
        input = nullptr;
    }

    void EditorWindow::Init(InputController* inputControl, ResourceController* resourceController)
    {
        input = new InputContext();
        inputController = inputControl;
        inputController->AddContext(Utilities::Format("EditorWindow {0}", input), input);
        NeuronGUI::Init(input, resourceController);
    }

    void EditorWindow::SetViewportRect(Rect rect)
    {
        settings.viewport = rect;
        if (renderer != nullptr)
        {
            renderer->SetViewportRect(rect.SDL());
        }
    }

    Rect EditorWindow::GetViewportRect()
    {
        return settings.viewport;
    }

    string EditorWindow::GetTitle()
    {
        return settings.title;
    }

    void EditorWindow::SetTitle(string title)
    {
        settings.title = title;
    }

    NativeEditorWindow::NativeEditorWindow(EditorWindow* root, InputController* controller, ResourceController* resources)
    {
        // Setup input and native window
        input = controller;
        windowContext = new InputContext();
        input->AddContext(Utilities::Format("NativeEditorWindow {0}", this), windowContext);
        native = windowContext->GetHandler<Window>();
        native->Init(root->settings.title.c_str(), root->settings.viewport.w, root->settings.viewport.h, false, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        // TODO: make this a method of the Window class
        SDL_SetWindowMinimumSize(native->GetWindowSDL(), (int)MIN_DIMENSIONS.x, (int)MIN_DIMENSIONS.y);

        // Now initialise the layout with a single root editor window
        root->node = layout.Insert(root);
        native->SetTitle(root->GetTitle());
        root->native = native;
        root->renderer = new Renderer(native);
        root->Init(input, resources);

        // Cleanup if the initial editor window gets destroyed
        root->OnDestroy += [&] (const EditorWindow& caller) { delete root->renderer; root->renderer = nullptr; layout.Remove(root->node); };
    }

    NativeEditorWindow::~NativeEditorWindow()
    {
        layout.Clear();
        input->RemoveContext(Utilities::Format("NativeEditorWindow {0}", this));
        delete windowContext;
    }

    void NativeEditorWindow::Update()
    {
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
    }

    void NativeEditorWindow::Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode)
    {
        if (dest->node == nullptr)
        {
            Logger::EngineLog().Error("Cannot dock editor window to window that has a NULL layout node!");
            return;
        }

        Rect viewport = source->GetViewportRect();
        Rect destViewport = dest->GetViewportRect();
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
            destViewport.w = destViewport.w - viewport.w;
            if (destViewport.h < MIN_DIMENSIONS.y)
            {
                // TODO: handle cases where the inserted window is too big
            }
            viewport.x = destViewport.x + (mode == DockingMode::BOTTOM ? destViewport.w : -destViewport.w);
            viewport.y = destViewport.y;
        }
        source->SetViewportRect(viewport);
        dest->SetViewportRect(destViewport);

        source->renderer = new Renderer(native);
        source->OnDestroy += [&] (const EditorWindow& caller) { delete source->renderer; source->renderer = nullptr; layout.Remove(source->node); };
    }

    void NativeEditorWindow::Remove(EditorWindow* source)
    {
    }

}
