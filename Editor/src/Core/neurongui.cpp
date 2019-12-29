#include "neurongui.h"

using namespace Ossium;

namespace Ossium::Editor
{

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
        OnGUI();
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

    void NeuronGUI::TextLabel(string text, const TextStyle& style)
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

    string NeuronGUI::TextField(string text, const TextStyle& style, SDL_Color bg, SDL_Color outlineColor, SDL_Color cursorColor)
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

    bool NeuronGUI::Button(string text)
    {
        return Button(text, styleButtonText);
    }

    bool NeuronGUI::Button(string text, const TextStyle& style)
    {
        if (IsVisible())
        {
            MouseHandler* mouse = input->GetHandler<MouseHandler>();

            // Create the texture from scratch
            Image texture;
            int fontSizes[2] = {1, style.ptsize};
            texture.CreateFromText(*renderer, *resources->Get<Font>(style.fontPath, fontSizes), text, style, (Uint32)renderer->GetWidth());

            // Set the destination rect
            SDL_Rect dest;
            dest.x = GetLayoutPosition().x + 2;
            dest.y = GetLayoutPosition().y + 2;
            dest.w = texture.GetWidth();
            dest.h = texture.GetHeight();

            Rect buttonDest = Rect(dest.x - 2, dest.y - 2, dest.w + 4, dest.h + 4);

            Vector2 mpos = mouse->GetMousePosition();

            // Render the button
            // TODO: generate nicer buttons and pass styling arguments
            buttonDest.DrawFilled(
                *renderer,
                buttonDest.Contains(mpos) ?
                    (mouse->LeftPressed() ?
                        Color(0, 180, 180) : // Pressed colour
                        Color(0, 255, 255)   // Hovered colour
                    ) : Color(0, 220, 220)  // Not hovered colour
            );

            // Button outline
            buttonDest.Draw(*renderer, Colors::BLACK);

            // Render the text
            texture.Render(renderer->GetRendererSDL(), dest);

            // Move along
            Move(GetLayoutDirection() ? dest.h : dest.w);

            return buttonDest.Contains(mpos) && DidClick(mpos);
        }
        return false;
    }

    bool NeuronGUI::Toggle(bool toggleValue)
    {
        // TODO
        return false;
    }

}
