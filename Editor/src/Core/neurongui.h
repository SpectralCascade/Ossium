#ifndef NEURONGUI_H
#define NEURONGUI_H

#include <stack>
#include <Ossium.h>

using namespace Ossium;
using namespace std;

namespace Ossium::Editor
{

    enum NeuronLayoutDirection
    {
        NEURON_LAYOUT_HORIZONTAL = 0,
        NEURON_LAYOUT_VERTICAL = 1
    };

    /// Provides immediate-mode GUI methods to derivative classes for fundamental UI elements and layouts.
    class NeuronGUI : public TextFormat
    {
    private:
        /// Used to determine how GUI elements are positioned.
        stack<Vector2> layoutStack;

        /// Used to determine whether elements should be positioned vertically or horizontally.
        stack<bool> layoutDirection;

        /// The current scrolled position of the GUI view.
        Vector2 scrollPos = Vector2(0, 0);

    protected:
        Renderer* renderer = nullptr;
        InputContext* input = nullptr;
        ResourceController* resources = nullptr;

    public:
        CONSTRUCT_SCHEMA(SchemaRoot, TextFormat);

        /// Requires a renderer instance to enqueue graphics, an input context for input and access to resources.
        /// TODO: make an empty default constructor that is safe to use
        NeuronGUI(Renderer* render, InputContext* inputContext, ResourceController* resourceController);

    private:
        void BeginLayout(int direction);
        void EndLayout();

    protected:
        /// Resets layout
        void Begin();

        /// Returns false if the current UI layout position is off screen.
        bool IsVisible();

        /// Increases the current position along x (if horizontal) or y (if vertical). Negative values are clamped to 0.
        void Move(float amount);

        /// Returns the current position.
        Vector2 GetLayoutPosition();

        /// Returns the current direction.
        int GetLayoutDirection();

        /// Begins positioning GUI elements horizontally.
        void BeginHorizontal();
        /// Stops positioning GUI elements horizontally.
        void EndHorizontal();

        /// Begins positioning GUI elements vertically.
        void BeginVertical();
        /// Stops positioning GUI elements vertically.
        void EndVertical();

        /// Displays some text.
        void TextLabel(string text);

        /// Takes text input and displays it. Returns the current string input.
        string TextField(string text);

        /// Displays a button that takes input. When a user activates the button, this returns true.
        bool Button(string buttonText = "Button");

        /// Displays a toggle button. When the toggleValue argument is true, appears enabled, otherwise appears disabled.
        bool Toggle(bool toggleValue, string labelText = "");

        /// Displays a drop-down list of items when clicked. Returns the selected item.
        /*template<typename T>
        vector<typename T>::iterator Dropdown(vector<typename T>::iterator selected, const vector<typename T>& values)
        {
            // TODO
            return selected;
        }*/

    };

}

#endif // NEURONGUI_H
