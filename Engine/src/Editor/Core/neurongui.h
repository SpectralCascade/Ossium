/** COPYRIGHT NOTICE
 *
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
#ifndef NEURONGUI_H
#define NEURONGUI_H

#include <stack>
#include "../../Ossium.h"

using namespace Ossium;
using namespace std;

namespace Ossium::Editor
{

    enum NeuronLayoutDirection
    {
        NEURON_LAYOUT_HORIZONTAL = 0,
        NEURON_LAYOUT_VERTICAL = 1
    };

    struct NeuronSkinSchema : public Schema<NeuronSkinSchema, 20>
    {
        DECLARE_BASE_SCHEMA(NeuronSkinSchema, 20);

        // Default text styles, used when no text style is specified.
        M(TextStyle, styleLabel);
        M(TextStyle, styleTextField);
        M(TextStyle, styleDropdownText);
        M(TextStyle, styleButtonText);
        M(SDL_Color, backgroundColor) = Color(200, 200, 200);
    };

    struct NeuronClickableStyle : public Schema<NeuronClickableStyle, 10>
    {
        DECLARE_BASE_SCHEMA(NeuronClickableStyle, 10);

        NeuronClickableStyle(
            SDL_Color bodyColor,
            TextStyle textStyle
        );

        NeuronClickableStyle(
            SDL_Color bodyColor,
            TextStyle textStyle,
            SDL_Color outlineColor
        );

        NeuronClickableStyle(
            SDL_Color bodyColor,
            TextStyle textStyle,
            SDL_Color endEdgeColors,
            SDL_Color sideEdgeColors
        );

        NeuronClickableStyle(
            SDL_Color bodyColor,
            TextStyle textStyle,
            SDL_Color topColor,
            SDL_Color bottomColor,
            SDL_Color leftColor,
            SDL_Color rightColor
        );

        NeuronClickableStyle(
            SDL_Color bodyNormal,
            SDL_Color bodyHover,
            SDL_Color bodyClick,
            TextStyle textNormal,
            TextStyle textHover,
            TextStyle textClick,
            SDL_Color topColor,
            SDL_Color bottomColor,
            SDL_Color leftColor,
            SDL_Color rightColor
        );

        M(SDL_Color, normalColor);
        M(SDL_Color, hoverColor);
        M(SDL_Color, clickColor);

        M(TextStyle, normalTextStyle);
        M(TextStyle, hoverTextStyle);
        M(TextStyle, clickTextStyle);

        M(SDL_Color, topEdgeColor);
        M(SDL_Color, bottomEdgeColor);
        M(SDL_Color, leftEdgeColor);
        M(SDL_Color, rightEdgeColor);
    };

    namespace NeuronStyles
    {
        extern TextStyle NEURON_TEXT_NORMAL_STYLE;
        extern TextStyle NEURON_TEXT_INVERSE_STYLE;
        extern NeuronClickableStyle NEURON_BUTTON_STYLE;
        extern NeuronClickableStyle NEURON_SLIDER_STYLE;
        extern NeuronClickableStyle NEURON_DROPDOWN_ITEM_STYLE;
        extern NeuronClickableStyle NEURON_TEXTFIELD_STYLE;
        extern NeuronClickableStyle NEURON_CHECKBOX_STYLE;
    }

    /// Provides immediate-mode GUI methods to derivative classes for fundamental UI elements and layouts.
    // TODO: don't inherit TextFormat!!!!!!!!!!!!!!!!!!!!! instead maybe setup layout options member in personal schema
    class NeuronGUI : public NeuronSkinSchema
    {
    private:
        /// Used to determine how GUI elements are positioned.
        stack<Vector2> layoutStack;

        /// Used to determine whether elements should be positioned vertically or horizontally.
        stack<bool> layoutDirection;

        /// Used to determine how much to move after completing a horizontal or vertical layout group.
        stack<float> layoutDifference;

        /// Used for order-based idents for text fields.
        Uint32 textFieldCounter = 1;

        /// Corresponds to the text field currently being edited.
        /// This is reset when the viewport loses focus or the user clicks away.
        Uint32 activeTextFieldId = 0;

        /// The position of the text cursor in a text field.
        int textFieldCursorPos = 0;

        /// The current scrolled position of the GUI view.
        Vector2 scrollPos = Vector2(0, 0);

        Vector2 lastMousePos = Vector2(0, 0);
        bool mousePressed = false;

        /// Handles keyboard inputs such as backspace while a text field is in use.
        ActionOutcome HandleTextField(const KeyboardInput& key);

    protected:
        Renderer* renderer = nullptr;
        InputContext* input = nullptr;
        ResourceController* resources = nullptr;

    public:
        CONSTRUCT_SCHEMA(SchemaRoot, NeuronSkinSchema);

        /// Requires a renderer instance to enqueue graphics, an input context for input and access to resources.
        /// TODO: make an empty default constructor that is safe to use
        NeuronGUI(Renderer* render, InputContext* inputContext, ResourceController* resourceController);

        /// Reloads the GUI.
        void Refresh();

    private:
        /// Resets the layout. Called automatically by Refresh().
        void Begin();
        /// Sets up the layout such that additional GUI elements are aligned in a particular direction.
        void BeginLayout(int direction);
        /// Pops back to the previous layout.s=
        void EndLayout();

    protected:
        /// Returns true if the mouse position has not changed between pressing the left button and releasing it.
        bool DidClick(Vector2 pos);

        /// Abstract method that should contain all GUI logic.
        virtual void OnGUI() = 0;

        /// Returns false if the current UI layout position is off screen.
        bool IsVisible();

        /// Increases the current position along x (if horizontal) or y (if vertical). Negative values are clamped to 0.
        /// You should provide the full size on both x and y of the layout element you define.
        void Move(Vector2 amount);

        /// Similar to Move, but just adds space along the current layout direction.
        void Space(float amount);

        /// Similar to Space, but automatically moves along to the next multiple of the specified tab size.
        void Tab(int tabSize = 30);

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
        void TextLabel(string text, TextStyle style);

        /// Takes text input and displays it. Returns the current string input.
        string TextField(string text);
        string TextField(string text, NeuronClickableStyle style, SDL_Color cursorColor = Colors::BLACK);

        /// Displays a button that takes input. When a user activates the button, this returns true.
        bool Button(string text, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(string text, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(Image* image, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(Image* image, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);

        /// Displays a toggle button. When the toggleValue argument is true, appears enabled, otherwise appears disabled.
        bool Toggle(bool toggleValue, SDL_Color checkColor = Colors::BLACK);
        bool Toggle(bool toggleValue, NeuronClickableStyle style, Vector2 boxSize, SDL_Color checkColor = Colors::BLACK);

        float Slider(
             float sliderValue,
             float minValue = 0.0f,
             float maxValue = 1.0f,
             int length = 50,
             int buttonWidth = 8,
             int buttonHeight = 8,
             NeuronClickableStyle style = NeuronStyles::NEURON_SLIDER_STYLE,
             bool invertOutline = false,
             Uint32 xpadding = 4,
             Uint32 ypadding = 4
        );

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
