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
            14,
            Colors::BLACK,
            0,
            0,
            0,
            0,
            RENDERTEXT_BLEND_WRAPPED
        );
        TextStyle NEURON_TEXT_INVERSE_STYLE = TextStyle(
            "../assets/Orkney Regular.ttf",
            14,
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
        styleLabel.ptsize = 14;

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
    }

    void NeuronGUI::EndLayout()
    {
        if (layoutStack.size() > 1)
        {
            layoutStack.pop();
            layoutDirection.pop();
        }
    }

    bool NeuronGUI::IsVisible()
    {
        return GetLayoutPosition().x < renderer->GetWidth() + scrollPos.x && GetLayoutPosition().y < renderer->GetHeight() + scrollPos.y;
    }

    void NeuronGUI::Move(float amount)
    {
        amount = max(0.0f, amount);
        if (GetLayoutDirection() == NEURON_LAYOUT_HORIZONTAL)
        {
            layoutStack.top().x += amount;
        }
        else
        {
            layoutStack.top().y += amount;
        }
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
            Move(GetLayoutDirection() ? dest.h : dest.w);
        }
    }

    string NeuronGUI::TextField(string text)
    {
        return TextField(text, styleTextField);
    }

    string NeuronGUI::TextField(string text, TextStyle style, SDL_Color bg, SDL_Color outlineColor, SDL_Color cursorColor)
    {
        if (IsVisible())
        {
            // TODO: text field input
            text += "|";

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
            Move(GetLayoutDirection() ? dest.h : dest.w);
        }
        return text;
    }

    bool NeuronGUI::Button(string text, int xpadding, int ypadding)
    {
        return Button(text, NeuronStyles::NEURON_BUTTON_STYLE, xpadding, ypadding);
    }

    bool NeuronGUI::Button(string text, NeuronClickableStyle style, int xpadding, int ypadding)
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
            line.Draw(*renderer, pressed && hovered ? style.bottomEdgeColor : style.topEdgeColor);
            line.b = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, pressed && hovered ? style.rightEdgeColor : style.leftEdgeColor);
            line.a.x += buttonDest.w;
            line.b.x = line.a.x;
            line.Draw(*renderer, pressed && hovered ? style.leftEdgeColor : style.rightEdgeColor);
            line.a = Vector2(buttonDest.x, buttonDest.y + buttonDest.h);
            line.Draw(*renderer, pressed && hovered ? style.topEdgeColor : style.bottomEdgeColor);

            // Move along
            Move(GetLayoutDirection() ? buttonDest.h + 1 : buttonDest.w + 1);

            return hovered && DidClick(mpos);
        }
        return false;
    }

    bool NeuronGUI::Toggle(bool toggleValue)
    {
        // TODO
        return false;
    }

}
