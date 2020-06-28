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

    struct NeuronViewportSchema : public Schema<NeuronViewportSchema, 20>
    {
        DECLARE_BASE_SCHEMA(NeuronViewportSchema, 20);

        // Default text styles, used when no text style is specified.
        M(TextStyle, styleLabel);
        M(TextStyle, styleTextField);
        M(TextStyle, styleDropdownText);
        M(TextStyle, styleButtonText);
        M(SDL_Color, backgroundColor) = Color(200, 200, 200);
        M(SDL_Rect, viewport) = {0, 0, 0, 0};

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
        extern NeuronClickableStyle NEURON_CONTEXT_OPTION_STYLE;
    }

    /// Provides immediate-mode GUI methods to derivative classes for fundamental UI elements and layouts.
    class NeuronGUI : public NeuronViewportSchema
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

        /// The last position of the text cursor in a text field.
        Uint32 lastTextFieldCursorPos = 0;

        /// The change multiplier for cursor vertical position, corresponding to Y axis (where the top < 0, bottom > 0).
        int verticalCursorPosChange = 0;

        /// The cursor rect rendered when a text field is in use.
        Rect textFieldCursor;

        /// The last glyph data located in a text field
        GlyphLocation lastGlyphLocation = {GlyphMeta(), TextLine(), Vector2::Zero, 0, false};

        /// The current scrolled position of the GUI view.
        Vector2 scrollPos = Vector2::Zero;

        /// Contains input state information
        struct NeuronInputState : public Schema<NeuronInputState, 7>
        {
            DECLARE_BASE_SCHEMA(NeuronInputState, 7);

            /// The position of the mouse in the current Refresh() call.
            M(Vector2, mousePos) = Vector2::Zero;

            /// The position of the mouse in the previous Refresh() call.
            M(Vector2, lastMousePos) = Vector2::Zero;

            /// The position of the mouse when it last pressed down.
            M(Vector2, mouseDownPos) = Vector2::NegOneNegOne;

            /// The position of the mouse when it last released.
            M(Vector2, mouseUpPos) = Vector2::NegOneNegOne;

            /// Whether the mouse is currently pressed.
            M(bool, mousePressed) = false;

            /// Whether the mouse was pressed in the last Refresh() call.
            M(bool, mouseWasPressed) = false;

            /// True if the mouse is hovering over a text field.
            M(bool, textFieldHovered) = false;

        } input_state;

        /// When true, refresh on the next Update() call.
        bool update = true;

        /// Handles keyboard inputs such as backspace while a text field is in use.
        ActionOutcome HandleTextField(const KeyboardInput& key);

    protected:
        Renderer* renderer = nullptr;
        InputContext* input = nullptr;
        ResourceController* resources = nullptr;

        /// When true, refreshes every time Update() is called.
        bool alwaysUpdate = false;

        /// The position of the native window.
        Vector2 nativeOrigin = Vector2::Zero;

        /// Read only reference to the input state information
        NeuronInputState& InputState = input_state;

    public:
        CONSTRUCT_SCHEMA(SchemaRoot, NeuronViewportSchema);

        NeuronGUI() = default;
        virtual ~NeuronGUI() = default;

        /// Initialises everything. Must be called immediately after construction!
        void Init(InputContext* inputContext, ResourceController* resourceController);

        /// Called once initialisation is complete.
        virtual void OnInit();

        /// Refreshes the GUI if necessary. Set forceUpdate to true if the GUI must be updated regardless of the current state.
        void Update(bool forceUpdate = false);

        /// Sets the update flag to true, so the next Update() causes a refresh to occur.
        void TriggerUpdate();

        ActionOutcome OnMouseEvent(const MouseInput& m);
        ActionOutcome OnKeyboardEvent(const KeyboardInput& key);

        /// Reloads the GUI.
        virtual void Refresh();

    private:
        /// Sets up the layout such that additional GUI elements are aligned in a particular direction.
        void BeginLayout(int direction);
        /// Pops back to the previous layout.
        void EndLayout();

    protected:
        /// Resets the layout. Called automatically by Refresh().
        void Begin();

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

        /// Dumps an image at the current layout position.
        void PlaceImage(Image* image);

        /// Displays a button that takes input. When a user activates the button, this returns true.
        bool Button(string text, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(string text, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool useMaxWidth = false);
        bool Button(string text, TextLayout& textLayout, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool useMaxWidth = false);
        bool Button(Image* image, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(Image* image, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4);
        bool Button(int w, int h, NeuronClickableStyle style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, Image* image = nullptr, bool* isHovered = nullptr, bool* isPressed = nullptr);

        /// Behaves somewhat like a regular button... except it's invisible and it's position is absolute.
        bool InvisibleButton(Rect area);

        /// An invisible input area that returns the change in mouse position from when the mouse begins dragging.
        Vector2 DraggableArea(Rect area, bool absolute = false);

        /// Helper method for computing the inner destination rect of a button.
        SDL_Rect GetButtonDest(int w, int h, float xpadding, float ypadding);
        /// Helper method for computing the true rect of a button.
        Rect GetButtonRect(int w, int h, float xpadding = 4, float ypadding = 4);
        Rect GetButtonRect(SDL_Rect dest, float xpadding = 4, float ypadding = 4);

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
        /// TODO: show as overlay, take input control from the window while open.
        /*template<typename T>
        vector<typename T>::iterator Dropdown(vector<typename T>::iterator selected, const vector<typename T>& values)
        {
            Font& font = *resources->Get<Font>(style.fontPath, style.ptsize, *renderer);
            vector<TextLayout> textLayouts;

            // First, find the widest element in the list
            float biggestWidth = 0;
            for (auto itr : values)
            {
                TextLayout tlayout;
                Vector2 limits = Vector2(renderer->GetWidth() - layoutPos.x, renderer->GetHeight());
                tlayout.SetPointSize(style.ptsize);
                tlayout.SetBounds(limits);
                tlayout.mainColor = style.fg;
                tlayout.mainStyle = style.style;
                tlayout.SetText(*renderer, font, text, true);
                tlayout.Update(font);
                tlayout.Render(*renderer, font, layoutPos);

                textLayouts.push_back();
                biggestWidth = GetButtonRect(,, );
            }
            // Now create buttons for each element but at an even width.
            for (auto itr : values)
            {
            }
            return selected;
        }*/

    };

}

#endif // NEURONGUI_H
