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
#include "neurongui.h"
#include "../../Core/textinput.h"
#include "../../Core/mousecursor.h"

using namespace Ossium;

namespace Ossium::Editor
{

    //
    // NeuronClickableStyle
    //

    NeuronClickableStyle::NeuronClickableStyle(SDL_Color bodyColor, TextStyle textStyle)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalTextStyle = textStyle;
        hoverTextStyle = textStyle;
        clickTextStyle = textStyle;
        bottomEdgeColor = Colors::BLACK;
        rightEdgeColor = Colors::BLACK;
        topEdgeColor = Colors::WHITE;
        leftEdgeColor = Colors::WHITE;
    }

    NeuronClickableStyle::NeuronClickableStyle(SDL_Color bodyColor, TextStyle textStyle, SDL_Color outlineColor)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalTextStyle = textStyle;
        hoverTextStyle = textStyle;
        clickTextStyle = textStyle;
        bottomEdgeColor = outlineColor;
        rightEdgeColor = outlineColor;
        topEdgeColor = outlineColor;
        leftEdgeColor = outlineColor;
    }

    NeuronClickableStyle::NeuronClickableStyle(
        SDL_Color bodyColor, TextStyle textStyle, SDL_Color endEdgeColors, SDL_Color sideEdgeColors)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalTextStyle = textStyle;
        hoverTextStyle = textStyle;
        clickTextStyle = textStyle;
        bottomEdgeColor = endEdgeColors;
        rightEdgeColor = sideEdgeColors;
        topEdgeColor = endEdgeColors;
        leftEdgeColor = sideEdgeColors;
    }

    NeuronClickableStyle::NeuronClickableStyle(
        SDL_Color bodyColor,
        TextStyle textStyle,
        SDL_Color topColor,
        SDL_Color bottomColor,
        SDL_Color leftColor,
        SDL_Color rightColor)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalTextStyle = textStyle;
        hoverTextStyle = textStyle;
        clickTextStyle = textStyle;
        bottomEdgeColor = bottomColor;
        rightEdgeColor = rightColor;
        topEdgeColor = topColor;
        leftEdgeColor = leftColor;
    }

    NeuronClickableStyle::NeuronClickableStyle(
        SDL_Color bodyNormal,
        SDL_Color bodyHover,
        SDL_Color bodyClick,
        TextStyle textNormal,
        TextStyle textHover,
        TextStyle textClick,
        SDL_Color topColor,
        SDL_Color bottomColor,
        SDL_Color leftColor,
        SDL_Color rightColor)
    {
        normalColor = bodyNormal;
        hoverColor = bodyHover;
        clickColor = bodyClick;
        normalTextStyle = textNormal;
        hoverTextStyle = textHover;
        clickTextStyle = textClick;
        bottomEdgeColor = bottomColor;
        rightEdgeColor = rightColor;
        topEdgeColor = topColor;
        leftEdgeColor = leftColor;
    }

    namespace NeuronStyles
    {
        TextStyle NEURON_TEXT_NORMAL_STYLE = TextStyle(
            "assets/Orkney Regular.ttf",
            12,
            Colors::BLACK,
            0,
            0,
            0,
            0,
            RENDERTEXT_BLEND_WRAPPED
        );
        TextStyle NEURON_TEXT_INVERSE_STYLE = TextStyle(
            "assets/Orkney Regular.ttf",
            12,
            Colors::WHITE,
            0,
            0,
            0,
            0,
            RENDERTEXT_BLEND_WRAPPED
        );
        NeuronClickableStyle NEURON_BUTTON_STYLE = NeuronClickableStyle(
            Color(0, 255, 255),
            NEURON_TEXT_NORMAL_STYLE
        );
        NeuronClickableStyle NEURON_SLIDER_STYLE = NeuronClickableStyle(
            Color(0, 255, 255),
            NEURON_TEXT_NORMAL_STYLE
        );
        NeuronClickableStyle NEURON_TEXTFIELD_STYLE = NeuronClickableStyle(
            Color(205, 205, 205) - 40,
            Color(205, 205, 205),
            Color(205, 205, 205),
            NEURON_TEXT_NORMAL_STYLE,
            NEURON_TEXT_NORMAL_STYLE,
            NEURON_TEXT_NORMAL_STYLE,
            Colors::BLACK,
            Colors::WHITE,
            Colors::BLACK,
            Colors::WHITE
        );
        NeuronClickableStyle NEURON_DROPDOWN_ITEM_STYLE = NeuronClickableStyle(
            Color(0, 220, 220),
            Color(0, 200, 0),
            Color(0, 200, 0),
            NEURON_TEXT_NORMAL_STYLE,
            NEURON_TEXT_INVERSE_STYLE,
            NEURON_TEXT_INVERSE_STYLE,
            Colors::TRANSPARENT,
            Colors::TRANSPARENT,
            Colors::TRANSPARENT,
            Colors::TRANSPARENT
        );
        NeuronClickableStyle NEURON_CHECKBOX_STYLE = NeuronClickableStyle(
            Colors::WHITE,
            NEURON_TEXT_NORMAL_STYLE,
            Colors::BLACK,
            Colors::WHITE,
            Colors::BLACK,
            Colors::WHITE
        );
    }

    //
    // NeuronGUI
    //

    NeuronGUI::NeuronGUI(Renderer* render, InputContext* inputContext, ResourceController* resourceController)
    {
        renderer = render;
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
        input->GetHandler<MouseHandler>()->AddBindlessAction(
            [&] (const MouseInput& m) {
                if (activeTextFieldId != 0 && m.type != MOUSE_MOTION && m.type != MOUSE_WHEEL)
                {
                    // Stop text field input
                    activeTextFieldId = 0;
                    //Logger::EngineLog().Info("Unset active text field");
                    input->GetHandler<TextInputHandler>()->StopListening();
                }
                this->update = true;
                return ActionOutcome::Ignore;
            }
        );
        KeyboardHandler* keyboard = input->GetHandler<KeyboardHandler>();
        TextInputHandler* textinput = input->GetHandler<TextInputHandler>();
        // TODO: this is bad practice; bindless actions must be cleaned up when this object gets destroyed!
        keyboard->AddBindlessAction(
            [&] (const KeyboardInput& key) {
                return this->HandleTextField(key);
            }
        );
        // Don't need to store the handle because the callback is destroyed when this is destroyed
        textinput->AddBindlessAction([&] (const InputChar& c) { this->update = true; return ActionOutcome::Ignore; });

        // There should always be at least one element on the stack
        layoutStack.push(Vector2(0, 0));
        // The default direction is vertical
        layoutDirection.push(NEURON_LAYOUT_VERTICAL);
        // No difference at root as the layout can't be ended manually
        layoutDifference.push(0);
    }

    // TODO: refactor keyboard inputs to make them rebindable.
    ActionOutcome NeuronGUI::HandleTextField(const KeyboardInput& key)
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

    void NeuronGUI::Update()
    {
        if (update)
        {
            Refresh();
        }
    }

    void NeuronGUI::Refresh()
    {
        // Clear the entire viewport
        SDL_RenderClear(renderer->GetRendererSDL());
        // Reset state
        Begin();
        // Immediate-mode GUI i/o
        OnGUI();
        // Render GUI
        renderer->SetDrawColor(backgroundColor);
        SDL_RenderPresent(renderer->GetRendererSDL());
        // Set mouse cursor
        if (textFieldHovered)
        {
            MouseCursor::Set(SDL_SYSTEM_CURSOR_IBEAM);
        }
        else
        {
            MouseCursor::Set(SDL_SYSTEM_CURSOR_ARROW);
        }
    }

    void NeuronGUI::Begin()
    {
        textFieldHovered = false;
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

    bool NeuronGUI::DidClick(Vector2 pos)
    {
        bool didClick = mousePressed;
        mousePressed = input->GetHandler<MouseHandler>()->LeftPressed();
        Vector2 oldPos = lastMousePos;
        lastMousePos = pos;
        return didClick && !mousePressed && pos == oldPos;
    }

    void NeuronGUI::BeginLayout(int direction)
    {
        layoutStack.push(GetLayoutPosition());
        layoutDirection.push(direction);
        layoutDifference.push(0);
    }

    void NeuronGUI::EndLayout()
    {
        if (layoutStack.size() > 1)
        {
            layoutStack.pop();
            layoutDirection.pop();
        }
        if (layoutDifference.size() > 1)
        {
            Vector2 moveAmount;
            if (GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL)
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

    bool NeuronGUI::IsVisible()
    {
        return GetLayoutPosition().x < renderer->GetWidth() + scrollPos.x && GetLayoutPosition().y < renderer->GetHeight() + scrollPos.y;
    }

    void NeuronGUI::Move(Vector2 amount)
    {
        if (GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL)
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

    void NeuronGUI::Space(float amount)
    {
        Move(GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL ? Vector2(amount, 0.0f) : Vector2(0.0f, amount));
    }

    void NeuronGUI::Tab(int tabSize)
    {
        int amount = tabSize;
        if (GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL)
        {
            amount = tabSize - ((int)GetLayoutPosition().x % tabSize);
        }
        else
        {
            amount = tabSize - ((int)GetLayoutPosition().y % tabSize);
        }
        Space((float)amount);
    }

    Vector2 NeuronGUI::GetLayoutPosition()
    {
        return layoutStack.top();
    }

    int NeuronGUI::GetLayoutDirection()
    {
        return layoutDirection.top();
    }

    void NeuronGUI::BeginHorizontal()
    {
        BeginLayout(NEURON_LAYOUT_HORIZONTAL);
    }

    void NeuronGUI::EndHorizontal()
    {
        EndLayout();
    }

    void NeuronGUI::BeginVertical()
    {
        BeginLayout(NEURON_LAYOUT_VERTICAL);
    }

    void NeuronGUI::EndVertical()
    {
        EndLayout();
    }

    void NeuronGUI::TextLabel(string text)
    {
        TextLabel(text, styleLabel);
    }

    void NeuronGUI::TextLabel(string text, TextStyle style)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.fontPath, style.ptsize, *renderer);
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

    string NeuronGUI::TextField(string text)
    {
        return TextField(text, NeuronStyles::NEURON_TEXTFIELD_STYLE);
    }

    string NeuronGUI::TextField(string text, NeuronClickableStyle style, SDL_Color cursorColor)
    {
        if (IsVisible())
        {
            unsigned int originalTextSize = text.size();
            TextInputHandler* textinput = input->GetHandler<TextInputHandler>();
            //MouseHandler* mouse = input->GetHandler<MouseHandler>();
            if (activeTextFieldId == textFieldCounter)
            {
                // Accept text input
                text = textinput->GetText();
            }

            Font& font = *resources->Get<Font>(style.normalTextStyle.fontPath, style.normalTextStyle.ptsize, *renderer);
            TextLayout tlayout;
            Vector2 layoutPos = GetLayoutPosition();
            Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x - 4, renderer->GetHeight() - 4);
            tlayout.SetPointSize(style.normalTextStyle.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = style.normalTextStyle.fg;
            tlayout.mainStyle = style.normalTextStyle.style;
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
                            lastGlyphLocation.position.x + layoutPos.x + 2 + lastGlyphLocation.glyph.GetAdvance(),
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
                else if (lastTextFieldCursorPos != textinput->GetCursorIndex() || originalTextSize != text.size())
                {
                    lastGlyphLocation = tlayout.LocateGlyph(textinput->GetCursorIndex());
                    if (lastGlyphLocation.valid)
                    {
                        textFieldCursor = Rect(
                            lastGlyphLocation.position.x + layoutPos.x + 2 + lastGlyphLocation.glyph.GetAdvance(),
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
                }
            }

            // Check if mouse is hovering over the button, if so change the cursor to an I beam.
            MouseHandler* mouse = input->GetHandler<MouseHandler>();
            Vector2 mpos = mouse->GetMousePosition();
            Vector2 layoutSize = tlayout.GetSize();
            Rect buttonRect = GetButtonRect(layoutSize.x, layoutSize.y);
            if (buttonRect.Contains(mpos))
            {
                textFieldHovered = true;
            }

            // Check if user clicks on the text field in this frame
            if (Button(text, tlayout, style, false))
            {
                activeTextFieldId = textFieldCounter;
                //Logger::EngineLog().Info("Active text field set to {0}", activeTextFieldId);
                textinput->StartListening();
                MouseHandler* mouse = input->GetHandler<MouseHandler>();
                Vector2 mousePos = mouse->GetMousePosition();
                // Locate the closest glyph to the mouse
                lastGlyphLocation = tlayout.LocateGlyph(mousePos - (layoutPos + /* account for default padding */ Vector2(2, 2)));
                if (lastGlyphLocation.valid)
                {
                    textFieldCursor = Rect(lastGlyphLocation.position.x + layoutPos.x + 2, lastGlyphLocation.position.y + layoutPos.y + 2, 1, lastGlyphLocation.line.size.y);
                    textinput->SetCursorIndex(lastGlyphLocation.index);
                }
            }

            TextLabel(Utilities::Format("Cursor Index = {0}", textinput->GetCursorIndex()));

            if (activeTextFieldId == textFieldCounter)
            {
                // Draw the cursor
                textFieldCursor.DrawFilled(*renderer, cursorColor);
                // Update the text field cursor index
                lastTextFieldCursorPos = textinput->GetCursorIndex();
            }

        }
        textFieldCounter++;
        return text;
    }

    bool NeuronGUI::Button(string text, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        return Button(text, NeuronStyles::NEURON_BUTTON_STYLE, invertOutline, xpadding, ypadding);
    }

    bool NeuronGUI::Button(string text, NeuronClickableStyle style, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.normalTextStyle.fontPath, style.normalTextStyle.ptsize, *renderer);
            TextLayout tlayout;
            Vector2 layoutPos = GetLayoutPosition();
            Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x - xpadding, renderer->GetHeight() - ypadding);
            // TODO: support other text styles for hover and click?
            tlayout.SetPointSize(style.normalTextStyle.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = style.normalTextStyle.fg;
            tlayout.mainStyle = style.normalTextStyle.style;
            tlayout.SetText(*renderer, font, text, true);
            tlayout.Update(font);
            return Button(text, tlayout, style, invertOutline, xpadding, ypadding);
        }
        return false;
    }

    bool NeuronGUI::Button(string text, TextLayout& textLayout, NeuronClickableStyle style, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        if (IsVisible())
        {
            Font& font = *resources->Get<Font>(style.normalTextStyle.fontPath, style.normalTextStyle.ptsize, *renderer);
            Vector2 layoutPos = GetLayoutPosition();

            bool result = Button(textLayout.GetSize().x, textLayout.GetSize().y, style, invertOutline, xpadding, ypadding);
            textLayout.Render(*renderer, font, layoutPos + Vector2(xpadding / 2, ypadding / 2));
            return result;
        }
        return false;
    }

    bool NeuronGUI::Button(Image* image, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        return Button(image, NeuronStyles::NEURON_BUTTON_STYLE, invertOutline, xpadding, ypadding);
    }

    bool NeuronGUI::Button(Image* image, NeuronClickableStyle style, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        return Button(image->GetWidth(), image->GetHeight(), style, invertOutline, xpadding, ypadding, image);
    }

    bool NeuronGUI::Button(int w, int h, NeuronClickableStyle style, bool invertOutline, Uint32 xpadding, Uint32 ypadding, Image* image)
    {
        if (IsVisible())
        {
            MouseHandler* mouse = input->GetHandler<MouseHandler>();

            // Set the destination rect
            SDL_Rect dest = GetButtonDest(w, h, xpadding, ypadding);
            Rect buttonDest = GetButtonRect(dest, xpadding, ypadding);

            Vector2 mpos = mouse->GetMousePosition();
            bool hovered = buttonDest.Contains(mpos);
            bool pressed = mouse->LeftPressed();

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

            return hovered && DidClick(mpos);
        }
        return false;
    }

    SDL_Rect NeuronGUI::GetButtonDest(int w, int h, float xpadding, float ypadding)
    {
        SDL_Rect dest;
        dest.x = GetLayoutPosition().x + (xpadding / 2);
        dest.y = GetLayoutPosition().y + (ypadding / 2);
        dest.w = w;
        dest.h = h;
        return dest;
    }

    Rect NeuronGUI::GetButtonRect(int w, int h, float xpadding, float ypadding)
    {
        return GetButtonRect(GetButtonDest(w, h, xpadding, ypadding), xpadding, ypadding);
    }

    Rect NeuronGUI::GetButtonRect(SDL_Rect dest, float xpadding, float ypadding)
    {
        return Rect(dest.x - (xpadding / 2), dest.y - (ypadding / 2), dest.w + xpadding, dest.h + ypadding);
    }

    bool NeuronGUI::Toggle(bool toggleValue, SDL_Color checkColor)
    {
        return Toggle(toggleValue, NeuronStyles::NEURON_CHECKBOX_STYLE, Vector2(12, 12), checkColor);
    }

    bool NeuronGUI::Toggle(bool toggleValue, NeuronClickableStyle style, Vector2 boxSize, SDL_Color checkColor)
    {
        Vector2 rootPos = GetLayoutPosition();
        // Checkbox button
        if (Button("", style, false, (Uint32)boxSize.x, (Uint32)boxSize.y))
        {
            toggleValue = !toggleValue;
        }

        if (toggleValue)
        {
            // Do tick
            Line line(rootPos + Vector2(boxSize.x / 4, boxSize.y / 2), rootPos + Vector2(boxSize.x / 2, (boxSize.y / 6) * 5));
            line.Draw(*renderer, checkColor);
            line.a = Vector2(Vector2((boxSize.x / 4) * 3, boxSize.y / 6)) + rootPos;
            line.Draw(*renderer, checkColor);
        }

        return toggleValue;
    }

    float NeuronGUI::Slider(float sliderValue, float minValue, float maxValue, int length, int buttonWidth, int buttonHeight, NeuronClickableStyle style, bool invertOutline, Uint32 xpadding, Uint32 ypadding)
    {
        if (IsVisible())
        {
            MouseHandler* mouse = input->GetHandler<MouseHandler>();
            Vector2 mpos = mouse->GetMousePosition();

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

            Rect oldButtonDest = Rect(dest.x, dest.y, dest.w, dest.h);
            bool wasHovered = oldButtonDest.Contains(lastMousePos);

            // Check how much the slider has been moved in this frame and change the slider value accordingly
            if (mousePressed && (wasHovered || slotDest.Contains(lastMousePos)))
            {
                // Calculate using mouse movement change
                /*Vector2 diff = mpos - lastMousePos;
                float change = Utilities::Clamp((int)diff.x, -length, length);
                sliderValue = Utilities::Clamp(sliderValue + ((change / (float)length) * (maxValue - minValue)), minValue, maxValue);*/

                // Move directly to mouse x
                sliderValue = Utilities::Clamp(
                    ((((mpos.x - slotDest.x) / (float)length)) * (maxValue - minValue)) + minValue,
                    minValue,
                    maxValue
                );

                // Update the button rect
                dest.x = GetLayoutPosition().x + (xpadding / 2) + ((sliderValue - minValue) / (maxValue - minValue)) * (float)length;
            }

            Rect buttonDest = Rect(dest.x, dest.y, dest.w, dest.h);

            bool hovered = buttonDest.Contains(mpos);
            bool pressed = mouse->LeftPressed();

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
                resources->Get<Font>(style.normalTextStyle.fontPath, style.normalTextStyle.ptsize, *renderer)->GenerateFromText(
                    *renderer, Utilities::ToString(sliderValue), style.normalTextStyle, (Uint32)renderer->GetWidth()
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
                GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL ? slotDest.w + xpadding * 2 + valueText.GetWidth() + 4 : buttonDest.w + xpadding,
                buttonDest.h + ypadding * 2
            ));

            if (hovered || slotDest.Contains(mpos))
            {
                // Update the mouse pressed state
                DidClick(mpos);
            }

        }
        return sliderValue;
    }

}
