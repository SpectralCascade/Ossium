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
#include "editorgui.h"
#include "contextmenu.h"
#include "../../Core/textinput.h"
#include "../../Core/mousecursor.h"

using namespace std;
using namespace Ossium;

namespace Ossium::Editor
{

    //
    // EditorGUI
    //

    EditorGUI::~EditorGUI()
    {
        if (input != nullptr)
        {
        }
    }

    void EditorGUI::Init(InputContext* inputContext, ResourceController* resourceController)
    {
        input = inputContext;
        resources = resourceController;

        // Set default font and text formatting
        // TODO: set these inline???
        styleLabel.fontPath = "assets/Orkney Regular.ttf";
        styleLabel.rendermode = RENDERTEXT_BLEND_WRAPPED;
        styleLabel.ptsize = 12;

        styleTextField = styleLabel;
        styleDropdownText = styleLabel;
        styleButtonText = styleLabel;

        // Refresh whenever the mouse interacts.
        // TODO: this is bad practice; bindless actions must be cleaned up when this object gets destroyed!
        // Consider what might happen if this object gets destroyed but the mouse handler is still in use...
        mouseActionId = input->GetHandler<MouseHandler>()->AddBindlessAction(
            [&] (const MouseInput& m) {
                if (activeTextFieldId != 0 && m.type != MOUSE_MOTION && m.type != MOUSE_WHEEL)
                {
                    // Stop text field input
                    activeTextFieldId = 0;
                    //Log.Info("Unset active text field");
                    input->GetHandler<TextInputHandler>()->StopListening();
                }
                this->update = true;
                return ActionOutcome::Ignore;
            }
        );
        KeyboardHandler* keyboard = input->GetHandler<KeyboardHandler>();
        TextInputHandler* textinput = input->GetHandler<TextInputHandler>();
        // TODO: this is bad practice; bindless actions must be cleaned up when this object gets destroyed!
        keyboardActionId = keyboard->AddBindlessAction(
            [&] (const KeyboardInput& key) {
                return this->HandleTextField(key);
            }
        );
        // Don't need to store the handle because the callback is destroyed when this is destroyed
        textActionId = textinput->AddBindlessAction([&] (const InputChar& c) { this->update = true; return ActionOutcome::Ignore; });

        // There should always be at least one element on the stack
        layoutStack.push(Vector2(0, 0));
        // The default direction is vertical
        layoutDirection.push(EDITOR_LAYOUT_VERTICAL);
        // No difference at root as the layout can't be ended manually
        layoutDifference.push(0);

        OnInit();
    }

    void EditorGUI::OnInit()
    {
    }

    // TODO: refactor keyboard inputs to make them rebindable.
    ActionOutcome EditorGUI::HandleTextField(const KeyboardInput& key)
    {
        if (activeTextFieldId != 0)
        {
            TextInputHandler* textinput = input->GetHandler<TextInputHandler>();
            if (key.state)
            {
                switch (key.key)
                {
                    case SDLK_BACKSPACE:
                        // CTRL-Backspace clears entire alphanumeric blocks and whitespace blocks but not punctuation.
                        if (SDL_GetModState() & KMOD_LCTRL)
                        {
                            bool stop = false;
                            while (!stop)
                            {
                                string popped = textinput->PopChar().utf8;
                                if ((popped.empty() || !(popped[0] == ' ' && textinput->GetText().back() == ' ')) &&
                                    (popped[0] < '0' || (popped[0] > '9' && popped[0] < 'A') ||
                                    (popped[0] > 'Z' && popped[0] < 'a') || popped[0] > 'z'))
                                {
                                    stop = true;
                                }
                            }
                        }
                        else
                        {
                            textinput->PopChar();
                        }
                        update = true;
                        break;
                    case SDLK_LEFT:
                        textinput->SetCursorIndex(max(0, (int)textinput->GetCursorIndex() - 1));
                        update = true;
                        break;
                    case SDLK_RIGHT:
                        textinput->SetCursorIndex(textinput->GetCursorIndex() + 1);
                        update = true;
                        break;
                    case SDLK_UP:
                        verticalCursorPosChange--;
                        update = true;
                        break;
                    case SDLK_DOWN:
                        verticalCursorPosChange++;
                        update = true;
                        break;
                    /*case SDLK_RETURN:
                        textinput->SetText(textinput->GetText() + '\n');
                        break;*/
                    default:
                        break;
                }
            }
        }
        return ActionOutcome::Ignore;
    }

    void EditorGUI::Update(bool forceUpdate)
    {
        if (update || alwaysUpdate || forceUpdate)
        {
            Refresh();
            update = false;
        }
    }

    void EditorGUI::TriggerUpdate()
    {
        update = true;
    }

    void EditorGUI::Refresh()
    {
        // Set the current viewport
        renderer->SetViewportRect(viewport);
        if (input->HasHandler<MouseHandler>())
        {
            // Setup mouse input based on the current viewport
            MouseHandler* mouse = input->GetHandler<MouseHandler>();
            mouse->SetViewport(viewport);

            // The global origin of the mouse (relative to the viewport and the window position).
            Vector2 origin = nativeOrigin + Vector2(viewport.x, viewport.y);

            // Now grab mouse state information
            input_state.lastMousePos = input_state.mousePos;
            input_state.mousePos = mouse->GetGlobalMousePosition(origin);
            input_state.mouseWasPressed = input_state.mousePressed;
            input_state.mousePressed = mouse->LeftPressed();
            if (input_state.mousePressed != input_state.mouseWasPressed)
            {
                if (input_state.mousePressed)
                {
                    input_state.mouseDownPos = mouse->GetGlobalMousePosition(origin);
                }
                else
                {
                    input_state.mouseUpPos = mouse->GetGlobalMousePosition(origin);
                }
            }
        }
        // Reset state
        Begin();
        // Immediate-mode GUI i/o
        OnGUI();

    }

    void EditorGUI::Begin()
    {
        input_state.textFieldHovered = false;
        while (layoutStack.size() > 1)
        {
            layoutStack.pop();
            layoutDirection.pop();
        }
        if (layoutDifference.size() > 1)
        {
            // TODO: log warning once, the dev has probably forgotten to call EndHorizontal or EndVertical at some point.
            while (layoutDifference.size() > 1)
            {
                layoutDifference.pop();
            }
            layoutDifference.top() = 0;
        }
        layoutStack.top() = Vector2(0, 0);
        // Reset the text field id generator
        textFieldCounter = 1;
    }

    void EditorGUI::BeginLayout(int direction)
    {
        layoutStack.push(GetLayoutPosition());
        layoutDirection.push(direction);
        layoutDifference.push(0);
    }

    void EditorGUI::EndLayout()
    {
        if (layoutStack.size() > 1)
        {
            layoutStack.pop();
            layoutDirection.pop();
        }
        if (layoutDifference.size() > 1)
        {
            Vector2 moveAmount;
            if (GetLayoutDirection() == EDITOR_LAYOUT_HORIZONTAL)
            {
                moveAmount = Vector2(layoutDifference.top(), 0.0f);
            }
            else
            {
                moveAmount = Vector2(0.0f, layoutDifference.top());
            }
            layoutDifference.pop();
            Move(moveAmount);
        }
        else
        {
            // Log warning once, attempted to EndHorizontal or EndVertical to many times
        }
    }

    bool EditorGUI::IsVisible()
    {
        return GetLayoutPosition().x < renderer->GetWidth() + scrollPos.x && GetLayoutPosition().y < renderer->GetHeight() + scrollPos.y;
    }

    void EditorGUI::Move(Vector2 amount)
    {
        if (GetLayoutDirection() == EDITOR_LAYOUT_HORIZONTAL)
        {
            layoutStack.top().x += max(0.0f, amount.x);
            layoutDifference.top() = max(layoutDifference.top(), amount.y);
        }
        else
        {
            layoutStack.top().y += max(0.0f, amount.y);
            layoutDifference.top() = max(layoutDifference.top(), amount.x);
        }
    }

    void EditorGUI::Space(float amount)
    {
        Move(GetLayoutDirection() == EDITOR_LAYOUT_HORIZONTAL ? Vector2(amount, 0.0f) : Vector2(0.0f, amount));
    }

    void EditorGUI::Tab(int tabSize)
    {
        int amount = tabSize;
        if (GetLayoutDirection() == EDITOR_LAYOUT_HORIZONTAL)
        {
            amount = tabSize - ((int)GetLayoutPosition().x % tabSize);
        }
        else
        {
            amount = tabSize - ((int)GetLayoutPosition().y % tabSize);
        }
        Space((float)amount);
    }

    Vector2 EditorGUI::GetLayoutPosition()
    {
        return layoutStack.top();
    }

    int EditorGUI::GetLayoutDirection()
    {
        return layoutDirection.top();
    }

    void EditorGUI::BeginHorizontal()
    {
        BeginLayout(EDITOR_LAYOUT_HORIZONTAL);
    }

    void EditorGUI::EndHorizontal()
    {
        EndLayout();
    }

    void EditorGUI::BeginVertical()
    {
        BeginLayout(EDITOR_LAYOUT_VERTICAL);
    }

    void EditorGUI::EndVertical()
    {
        EndLayout();
    }

    float EditorGUI::GetCurrentBlockSize()
    {
        if (layoutDifference.size() > 1)
        {
            return layoutDifference.top();
        }
        return 0;
    }

    void EditorGUI::TextLabel(string text)
    {
        TextLabel(text, styleLabel);
    }

    void EditorGUI::TextLabel(string text, StyleText style)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.fontPath, style.ptsize);
            Vector2 layoutPos = GetLayoutPosition();
            TextLayout tlayout;
            Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x, renderer->GetHeight());
            tlayout.SetPointSize(style.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = style.fg;
            tlayout.mainStyle = style.style;
            tlayout.SetText(*renderer, font, text, true);
            tlayout.Update(font);
            tlayout.Render(*renderer, font, layoutPos);

            // Move along
            Move(tlayout.GetSize());
        }
    }

    string EditorGUI::TextField(string text)
    {
        return TextField(text, EditorStyle::EDITOR_TEXTFIELD_STYLE);
    }

    string EditorGUI::TextField(string text, StyleClickable style, SDL_Color cursorColor)
    {
        if (IsVisible())
        {
            TextInputHandler* textinput = input->GetHandler<TextInputHandler>();
            //MouseHandler* mouse = input->GetHandler<MouseHandler>();
            if (activeTextFieldId == textFieldCounter)
            {
                // Accept text input
                text = textinput->GetText();
            }

            Font& font = *resources->Get<Font>(style.normalStyleText.fontPath, style.normalStyleText.ptsize);
            TextLayout tlayout;
            Vector2 layoutPos = GetLayoutPosition();
            Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x - 4, renderer->GetHeight() - 4);
            tlayout.SetPointSize(style.normalStyleText.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = style.normalStyleText.fg;
            tlayout.mainStyle = style.normalStyleText.style;
            tlayout.SetText(*renderer, font, text, false);
            // Compute layout
            tlayout.Update(font);

            // Now compute the cursor position
            if (activeTextFieldId == textFieldCounter)
            {
                if (verticalCursorPosChange != 0)
                {
                    // Recompute the position of the cursor rect and the corresponding text index.
                    lastGlyphLocation = lastGlyphLocation.valid ? lastGlyphLocation : tlayout.LocateGlyph(textinput->GetCursorIndex());
                    if (lastGlyphLocation.valid)
                    {
                        lastGlyphLocation = tlayout.LocateGlyph(lastGlyphLocation.position + Vector2(0, verticalCursorPosChange * font.GetLineDifference(tlayout.GetPointSize())));
                    }
                    if (lastGlyphLocation.valid)
                    {
                        textinput->SetCursorIndex(lastGlyphLocation.index);
                        textFieldCursor = Rect(
                            lastGlyphLocation.position.x + layoutPos.x + 2,
                            lastGlyphLocation.position.y + layoutPos.y + 2,
                            1,
                            lastGlyphLocation.line.size.y
                        );
                    }
                    else
                    {
                        textFieldCursor.x = layoutPos.x + 2;
                        textFieldCursor.y = layoutPos.y + 2;
                    }
                    verticalCursorPosChange = 0;
                }
                else if (lastTextFieldCursorPos != textinput->GetCursorIndex())
                {
                    lastGlyphLocation = textinput->GetCursorIndex() < tlayout.GetTotalGlyphs() ? tlayout.LocateGlyph(textinput->GetCursorIndex()) : tlayout.LocateGlyph(textinput->GetCursorIndex() - 1);
                    if (lastGlyphLocation.valid)
                    {
                        textFieldCursor = Rect(
                            lastGlyphLocation.position.x + layoutPos.x + 2 + (textinput->GetCursorIndex() < tlayout.GetTotalGlyphs() ? 0 : lastGlyphLocation.glyph.GetAdvance(tlayout.GetPointSize())),
                            lastGlyphLocation.position.y + layoutPos.y + 2,
                            1,
                            lastGlyphLocation.line.size.y
                        );
                    }
                    else
                    {
                        textFieldCursor.x = layoutPos.x + 2;
                        textFieldCursor.y = layoutPos.y + 2;
                    }
                    lastTextFieldCursorPos = textinput->GetCursorIndex();
                }
            }

            // Check if mouse is hovering over the button, if so change the cursor to an I beam.
            Vector2 layoutSize = tlayout.GetSize();
            Rect buttonRect = GetButtonRect(layoutSize.x, layoutSize.y);
            if (buttonRect.Contains(InputState.mousePos))
            {
                input_state.textFieldHovered = true;
            }

            // Check if user clicks on the text field in this frame
            if (Button(text, tlayout, style, false))
            {
                activeTextFieldId = textFieldCounter;
                //Log.Info("Active text field set to {0}", activeTextFieldId);
                textinput->StartListening();
                textinput->SetText(text);
                // Locate the closest glyph to the mouse
                lastGlyphLocation = tlayout.LocateGlyph(InputState.mousePos - (layoutPos + /* account for default padding */ Vector2(2, 2)));
                if (lastGlyphLocation.valid)
                {
                    textFieldCursor = Rect(lastGlyphLocation.position.x + layoutPos.x + 2, lastGlyphLocation.position.y + layoutPos.y + 2, 1, lastGlyphLocation.line.size.y);
                    textinput->SetCursorIndex(lastGlyphLocation.index);
                }
            }

            //TextLabel(Utilities::Format("Cursor Index = {0}", textinput->GetCursorIndex()));

            if (activeTextFieldId == textFieldCounter)
            {
                // Draw the cursor
                textFieldCursor.DrawFilled(*renderer, cursorColor);
            }

        }
        textFieldCounter++;
        return text;
    }

    void EditorGUI::PlaceImage(Image* image)
    {
        Vector2 dimensions = Vector2(image->GetWidth(), image->GetHeight());
        image->Render(renderer->GetRendererSDL(), Rect(GetLayoutPosition().x, GetLayoutPosition().y, dimensions.x, dimensions.y).SDL());
        Move(dimensions);
    }

    bool EditorGUI::Button(string text, bool invertOutline, Uint32 xpadding, Uint32 ypadding, bool useMaxWidth, bool* isHovered, bool* isPressed)
    {
        return Button(text, EditorStyle::StandardButton, invertOutline, xpadding, ypadding, useMaxWidth, isHovered, isPressed);
    }

    bool EditorGUI::Button(string text, StyleClickable style, bool invertOutline, Uint32 xpadding, Uint32 ypadding, bool useMaxWidth, bool* isHovered, bool* isPressed)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.normalStyleText.fontPath, style.normalStyleText.ptsize);
            TextLayout tlayout;
            Vector2 layoutPos = GetLayoutPosition();
            Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x - xpadding, renderer->GetHeight() - ypadding);
            tlayout.SetPointSize(style.normalStyleText.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = style.normalStyleText.fg;
            tlayout.mainStyle = style.normalStyleText.style;
            tlayout.SetAlignment(useMaxWidth ? Typographic::TextAlignment::CENTERED : (Typographic::TextAlignment)style.normalStyleText.alignment);
            tlayout.SetText(*renderer, font, text, true);
            tlayout.Update(font);
            return Button(text, tlayout, style, invertOutline, xpadding, ypadding, useMaxWidth, isHovered, isPressed);
        }
        return false;
    }

    bool EditorGUI::Button(string text, TextLayout& textLayout, StyleClickable style, bool invertOutline, Uint32 xpadding, Uint32 ypadding, bool useMaxWidth, bool* isHovered, bool* isPressed)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.normalStyleText.fontPath, style.normalStyleText.ptsize);
            Vector2 layoutPos = GetLayoutPosition();

            bool hovered;
            bool pressed;
            if (isHovered == nullptr)
            {
                isHovered = &hovered;
            }
            if (isPressed == nullptr)
            {
                isPressed = &pressed;
            }
            bool result = Button(useMaxWidth ? viewport.w - xpadding : textLayout.GetSize().x, textLayout.GetSize().y, style, invertOutline, xpadding, ypadding, nullptr, isHovered, isPressed);
            if (*isHovered && (textLayout.mainColor != style.hoverStyleText.fg || textLayout.mainStyle != style.hoverStyleText.style))
            {
                textLayout.mainColor = style.hoverStyleText.fg;
                textLayout.mainStyle = style.hoverStyleText.style;
                textLayout.SetText(*renderer, font, text, true);
            }
            else if (*isPressed && (textLayout.mainColor != style.clickStyleText.fg || textLayout.mainStyle != style.clickStyleText.style))
            {
                textLayout.mainColor = style.clickStyleText.fg;
                textLayout.mainStyle = style.clickStyleText.style;
                textLayout.SetText(*renderer, font, text, true);
            }
            textLayout.Update(font);
            textLayout.Render(*renderer, font, layoutPos + Vector2(xpadding / 2, ypadding / 2));
            return result;
        }
        return false;
    }

    bool EditorGUI::Button(Image* image, bool invertOutline, Uint32 xpadding, Uint32 ypadding, bool* isHovered, bool* isPressed)
    {
        return Button(image, EditorStyle::StandardButton, invertOutline, xpadding, ypadding, isHovered, isPressed);
    }

    bool EditorGUI::Button(Image* image, StyleClickable style, bool invertOutline, Uint32 xpadding, Uint32 ypadding, bool* isHovered, bool* isPressed)
    {
        return Button(image->GetWidth(), image->GetHeight(), style, invertOutline, xpadding, ypadding, image, isHovered, isPressed);
    }

    bool EditorGUI::Button(int w, int h, StyleClickable style, bool invertOutline, Uint32 xpadding, Uint32 ypadding, Image* image, bool* isHovered, bool* isPressed)
    {
        if (IsVisible())
        {
            // Set the destination rect
            SDL_Rect dest = GetButtonDest(w, h, xpadding, ypadding);
            Rect buttonDest = GetButtonRect(dest, xpadding, ypadding);

            bool hovered = buttonDest.Contains(InputState.mousePos);
            bool pressed = InputState.mousePressed && buttonDest.Contains(InputState.mouseDownPos);
            if (isHovered != nullptr)
            {
                *isHovered = hovered;
            }
            if (isPressed != nullptr)
            {
                *isPressed = pressed;
            }

            SDL_Color buttonColour = hovered ?
                (pressed ?
                    style.clickColor : // Pressed colour
                    style.hoverColor   // Hovered colour
                ) : style.normalColor; // Not hovered colour

            // Render the button
            // TODO: generate nicer buttons and pass styling arguments
            buttonDest.DrawFilled(*renderer, buttonColour);

            if (image != nullptr)
            {
                // Render the image
                image->Render(renderer->GetRendererSDL(), dest);
            }

            // Button outline
            Line line(Vector2(buttonDest.x, buttonDest.y), Vector2(buttonDest.x + buttonDest.w, buttonDest.y));
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.bottomEdgeColor : style.topEdgeColor);
            line.b = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.rightEdgeColor : style.leftEdgeColor);
            line.a.x += buttonDest.w;
            line.b.x = line.a.x;
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.leftEdgeColor : style.rightEdgeColor);
            line.a = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.topEdgeColor : style.bottomEdgeColor);

            // Move along
            Move(Vector2(buttonDest.w + 1, buttonDest.h + 1));

            return hovered && !InputState.mousePressed && InputState.mouseWasPressed && buttonDest.Contains(InputState.mouseDownPos);
        }
        return false;
    }

    bool EditorGUI::InvisibleButton(Rect area)
    {
        bool hovered = area.Contains(InputState.mousePos);

        return hovered && !InputState.mousePressed && InputState.mouseWasPressed && area.Contains(InputState.mouseDownPos);
    }

    Vector2 EditorGUI::DraggableArea(Rect area, bool absolute)
    {
        return Vector2::Zero;
    }

    SDL_Rect EditorGUI::GetButtonDest(int w, int h, float xpadding, float ypadding)
    {
        SDL_Rect dest;
        dest.x = GetLayoutPosition().x + (xpadding / 2);
        dest.y = GetLayoutPosition().y + (ypadding / 2);
        dest.w = w;
        dest.h = h;
        return dest;
    }

    Rect EditorGUI::GetButtonRect(int w, int h, float xpadding, float ypadding)
    {
        return GetButtonRect(GetButtonDest(w, h, xpadding, ypadding), xpadding, ypadding);
    }

    Rect EditorGUI::GetButtonRect(SDL_Rect dest, float xpadding, float ypadding)
    {
        return Rect(dest.x - (xpadding / 2), dest.y - (ypadding / 2), dest.w + xpadding, dest.h + ypadding);
    }

    bool EditorGUI::Toggle(bool toggleValue, SDL_Color checkColor)
    {
        return Toggle(toggleValue, EditorStyle::EDITOR_CHECKBOX_STYLE, Vector2(12, 12), checkColor);
    }

    bool EditorGUI::Toggle(bool toggleValue, StyleClickable style, Vector2 boxSize, SDL_Color checkColor)
    {
        Vector2 rootPos = GetLayoutPosition();
        return Toggle(
            toggleValue,
            style,
            boxSize,
            [&] (bool on) {
                if (on)
                {
                    // Do tick
                    Line line(rootPos + Vector2(boxSize.x / 4, boxSize.y / 2), rootPos + Vector2(boxSize.x / 2, (boxSize.y / 6) * 5));
                    line.Draw(*renderer, checkColor);
                    line.a = Vector2(Vector2((boxSize.x / 4) * 3, boxSize.y / 6)) + rootPos;
                    line.Draw(*renderer, checkColor);
                }
            }
        );
    }

    bool EditorGUI::Toggle(bool toggleValue, StyleClickable style, Vector2 boxSize, function<void(bool)> drawFunc)
    {
        if (IsVisible())
        {
            // Empty button
            if (Button("", style, false, (Uint32)boxSize.x, (Uint32)boxSize.y))
            {
                toggleValue = !toggleValue;
            }

            // Draw whatever we want on top
            drawFunc(toggleValue);
        }
        return toggleValue;
    }

    float EditorGUI::Slider(float sliderValue, float minValue, float maxValue, int length, int buttonWidth, int buttonHeight, StyleClickable style, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        if (IsVisible())
        {
            // Draw the slot first
            Rect slotDest = Rect(
                GetLayoutPosition().x + (xpadding / 2) + (buttonWidth / 2),
                GetLayoutPosition().y + ypadding + (buttonHeight / 4),
                (float)length,
                buttonHeight / 2
            );

            // Render the slider slot
            slotDest.DrawFilled(*renderer, Color(backgroundColor.r / 2, backgroundColor.g / 2, backgroundColor.b / 2));

            // Slot outline
            Line line(Vector2(slotDest.x, slotDest.y), Vector2(slotDest.x + slotDest.w, slotDest.y));
            line.Draw(*renderer, Alpha(style.bottomEdgeColor, 200));
            line.b = Vector2(slotDest.x, slotDest.y + slotDest.h);
            line.Draw(*renderer, Alpha(style.rightEdgeColor, 200));
            line.a.x += slotDest.w;
            line.b.x = line.a.x;
            line.Draw(*renderer, Alpha(style.leftEdgeColor, 200));
            line.a = Vector2(slotDest.x, slotDest.y + slotDest.h);
            line.Draw(*renderer, Alpha(style.topEdgeColor, 200));

            // Get the current button rect and last mouse state data
            SDL_Rect dest;
            dest.x = GetLayoutPosition().x + (xpadding / 2) + ((sliderValue - minValue) / (maxValue - minValue)) * (float)length;
            dest.y = GetLayoutPosition().y + ypadding;
            dest.w = buttonWidth;
            dest.h = buttonHeight;

            Rect oldButtonDest = Rect(dest);
            if (InputState.mousePressed && !InputState.mouseWasPressed && oldButtonDest.Contains(InputState.mouseDownPos))
            {
                // Fake the 'mouse down position' because the button destination is not static, but the slot is (so it can be relied upon somewhat).
                input_state.mouseDownPos.x = slotDest.x;
                input_state.mouseDownPos.y = slotDest.y;
            }

            bool dragging = InputState.mousePressed && slotDest.Contains(InputState.mouseDownPos);

            // Check how much the slider has been moved in this frame and change the slider value accordingly
            if (dragging)
            {
                // Move directly to mouse x
                sliderValue = Utilities::Clamp(
                    ((((InputState.mousePos.x - slotDest.x) / (float)length)) * (maxValue - minValue)) + minValue,
                    minValue,
                    maxValue
                );

                // Update the button rect
                dest.x = GetLayoutPosition().x + (xpadding / 2) + ((sliderValue - minValue) / (maxValue - minValue)) * (float)length;
            }

            Rect buttonDest = Rect(dest);

            bool hovered = buttonDest.Contains(InputState.mousePos) || dragging;
            bool pressed = dragging;

            SDL_Color buttonColour = hovered ?
                (pressed ?
                    style.clickColor : // Pressed colour
                    style.hoverColor   // Hovered colour
                ) : style.normalColor; // Not hovered colour

            // Render the button
            buttonDest.DrawFilled(*renderer, buttonColour);

            // Button outline
            line = Line(Vector2(buttonDest.x, buttonDest.y), Vector2(buttonDest.x + buttonDest.w, buttonDest.y));
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.bottomEdgeColor : style.topEdgeColor);
            line.b = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.rightEdgeColor : style.leftEdgeColor);
            line.a.x += buttonDest.w;
            line.b.x = line.a.x;
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.leftEdgeColor : style.rightEdgeColor);
            line.a = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, invertOutline && pressed && hovered ? style.topEdgeColor : style.bottomEdgeColor);

            // Draw float value
            Image valueText;
            valueText.SetSurface(
                resources->Get<Font>(style.normalStyleText.fontPath, style.normalStyleText.ptsize)->GenerateFromText(
                    Utilities::ToString(sliderValue), style.normalStyleText, (Uint32)renderer->GetWidth()
                )
            );
            valueText.PushGPU(*renderer);
            SDL_Rect textDest;
            textDest.w = valueText.GetWidth();
            textDest.h = valueText.GetHeight();
            textDest.x = slotDest.x + slotDest.w + buttonDest.w + 2;
            textDest.y = slotDest.y - (textDest.h / 2) + (slotDest.h / 2);

            valueText.Render(renderer->GetRendererSDL(), textDest);

            // Move along
            Move(Vector2(
                GetLayoutDirection() == EDITOR_LAYOUT_HORIZONTAL ? slotDest.w + xpadding * 2 + valueText.GetWidth() + 4 : buttonDest.w + xpadding,
                buttonDest.h + ypadding * 2
            ));

        }
        return sliderValue;
    }

    string EditorGUI::DragAndDropField(string currentInput, bool* changed)
    {
        if (IsVisible())
        {
            bool hovered = false;
            bool pressed = false;
            Button(currentInput, false, 4, 4, true, &hovered, &pressed);
            if (hovered)
            {
                if (pressed)
                {
                    // TODO: Set drag & drop state string to currentInput
                }
                else if (InputState.mouseWasPressed && !InputState.mousePressed)
                {
                    // TODO: Set currentInput to drag & drop state string
                    //currentInput = ;
                    *changed = true;
                }

            }

        }
        return currentInput;
    }

    void EditorGUI::Dropdown(unsigned int selected, vector<string>& values, function<void(unsigned int)> onSelectHandler)
    {
        if (IsVisible())
        {
            Vector2 oldPos = GetLayoutPosition();
            if (Button(values[selected]))
            {
                OnDropdown(
                    Vector2(oldPos.x, GetLayoutPosition().y) + renderer->GetWindow()->GetPosition(),
                    values,
                    onSelectHandler
                );
            }
        }
    }

    void EditorGUI::OnDropdown(Vector2 position, vector<string>& names, function<void(unsigned int)> onSelectHandler)
    {
        ContextMenu& menu = *ContextMenu::GetMainInstance(resources);
        menu.ClearOptions();
        for (unsigned int i = 0, counti = names.size(); i < counti; i++)
        {
            menu.Add(names[i], [i, onSelectHandler, names] () { onSelectHandler(i); Log.Info("Option \"{0}\" [{1}] clicked.", names[i], i); });
        }
        menu.Show(position + Vector2(viewport.x, viewport.y));
    }

}
