#include "neurongui.h"

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
            "../assets/Orkney Regular.ttf",
            12,
            Colors::BLACK,
            0,
            0,
            0,
            0,
            RENDERTEXT_BLEND_WRAPPED
        );
        TextStyle NEURON_TEXT_INVERSE_STYLE = TextStyle(
            "../assets/Orkney Regular.ttf",
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
        styleLabel.fontPath = "../assets/Orkney Regular.ttf";
        styleLabel.rendermode = RENDERTEXT_BLEND_WRAPPED;
        styleLabel.ptsize = 12;

        styleTextField = styleLabel;
        styleDropdownText = styleLabel;
        styleButtonText = styleLabel;

        // Refresh whenever the mouse interacts.
        input->GetHandler<MouseHandler>()->AddBindlessAction(
            [&] (const MouseInput& m) { this->Refresh(); return ActionOutcome::Ignore; }
        );

        // There should always be at least one element on the stack
        layoutStack.push(Vector2(0, 0));
        // The default direction is vertical
        layoutDirection.push(NEURON_LAYOUT_VERTICAL);
        // No difference at root as the layout can't be ended manually
        layoutDifference.push(0);
    }

    void NeuronGUI::Refresh()
    {
        SDL_RenderClear(renderer->GetRendererSDL());
        OnGUI();
        renderer->SetDrawColor(Color(200, 200, 200));
        SDL_RenderPresent(renderer->GetRendererSDL());
    }

    void NeuronGUI::Begin()
    {
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
            // Create the texture from scratch
            Image texture;
            int fontSizes[2] = {1, style.ptsize};
            texture.CreateFromText(*renderer, *resources->Get<Font>(style.fontPath, fontSizes), text, style, (Uint32)renderer->GetWidth());

            // Set the destination rect
            SDL_Rect dest;
            dest.x = GetLayoutPosition().x;
            dest.y = GetLayoutPosition().y;
            dest.w = texture.GetWidth();
            dest.h = texture.GetHeight();

            // Render the texture
            texture.Render(renderer->GetRendererSDL(), dest);

            // Move along
            Move(Vector2(dest.w, dest.h));
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
            text += "|";

            if (Button(text, style, false))
            {
                // Start text input
            }

        }
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
            MouseHandler* mouse = input->GetHandler<MouseHandler>();

            // TODO: support other text styles for hover and click?
            TextStyle textStyle = style.normalTextStyle;

            // Create the texture from scratch
            Image texture;
            int fontSizes[2] = {1, textStyle.ptsize};
            texture.CreateFromText(*renderer, *resources->Get<Font>(textStyle.fontPath, fontSizes), text, textStyle, (Uint32)renderer->GetWidth());

            // Set the destination rect
            SDL_Rect dest;
            dest.x = GetLayoutPosition().x + (xpadding / 2);
            dest.y = GetLayoutPosition().y + (ypadding / 2);
            dest.w = texture.GetWidth();
            dest.h = texture.GetHeight();

            Rect buttonDest = Rect(dest.x - (xpadding / 2), dest.y - (ypadding / 2), dest.w + xpadding, dest.h + ypadding);

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

            // Render the text
            texture.Render(renderer->GetRendererSDL(), dest);

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

}
