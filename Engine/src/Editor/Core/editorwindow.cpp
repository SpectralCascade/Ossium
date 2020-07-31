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
#include "editorlayout.h"
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
            native = nativeWindow;
            EditorGUI::Init(input, nativeWindow->GetResources());
        }
    }

    EditorLayout* EditorWindow::GetEditorLayout()
    {
        return native;
    }

    Vector2 EditorWindow::GetNativePosition(bool relative)
    {
        return (relative ? GetEditorLayout()->GetNativeWindow()->GetRelativePosition() : GetEditorLayout()->GetNativeWindow()->GetPosition()) + Vector2(viewport.x, viewport.y);
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

        EditorGUI::Refresh();

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
            Rect left = Rect(rect.x, rect.y, max(padding, 4), rect.h);
            Rect top = Rect(rect.x, rect.y, rect.w, max(padding, 4));
            Rect right = Rect(rect.x + rect.w - max(padding, 4), rect.y, max(padding, 4), rect.h);
            Rect bottom = Rect(rect.x, rect.y + rect.h - max(padding, 4), rect.w, max(padding, 4));

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

}
