#include "neurongui.h"

using namespace Ossium;

namespace Ossium::Editor
{

    NeuronGUI::NeuronGUI(Renderer* render, InputContext* inputContext, ResourceController* resourceController)
    {
        renderer = render;
        input = inputContext;
        resources = resourceController;
        // Set default font
        ptsize = 24;
        fontPath = "../assets/Orkney Regular.ttf";
        // There should always be at least one element on the stack
        layoutStack.push(Vector2(0, 0));
        // The default direction is vertical
        layoutDirection.push(NEURON_LAYOUT_VERTICAL);
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
        if (IsVisible())
        {
            // Create the texture from scratch
            Image texture;
            texture.CreateFromText(*renderer, *resources->Get<Font>(fontPath), text, ptsize, fg, hinting, kerning, outline, style, rendermode, bg);

            // Set the destination rect
            SDL_Rect dest;
            dest.x = GetLayoutPosition().x;
            dest.y = GetLayoutPosition().y;
            dest.w = texture.GetWidth();
            dest.h = texture.GetHeight();

            // Render the texture
            texture.Render(renderer->GetRendererSDL(), dest);

            // Move along
            Move(dest.h + 4);
        }
    }

    string NeuronGUI::TextField(string text)
    {
        // TODO
    }

    bool NeuronGUI::Button(string buttonText)
    {
        // TODO
    }

    bool NeuronGUI::Toggle(bool toggleValue, string labelText)
    {
        // TODO
    }

}
