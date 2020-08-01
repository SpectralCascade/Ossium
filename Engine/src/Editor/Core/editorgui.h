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
#ifndef EDITORGUI_H
#define EDITORGUI_H

#include <stack>
#include "../../Ossium.h"
#include "editorstyle.h"

using namespace Ossium;

namespace Ossium::Editor
{

    enum EditorLayoutDirection
    {
        EDITOR_LAYOUT_HORIZONTAL = 0,
        EDITOR_LAYOUT_VERTICAL = 1
    };

    struct EditorViewportSchema : public Schema<EditorViewportSchema, 20>
    {
        DECLARE_BASE_SCHEMA(EditorViewportSchema, 20);

        // Default text styles, used when no text style is specified.
        M(StyleText, styleLabel);
        M(StyleText, styleTextField);
        M(StyleText, styleDropdownText);
        M(StyleText, styleButtonText);
        M(SDL_Color, backgroundColor) = Color(200, 200, 200);
        M(SDL_Rect, viewport) = {0, 0, 0, 0};

    };

    /// Provides immediate-mode GUI methods to derivative classes for fundamental UI elements and layouts.
    class EditorGUI : public EditorViewportSchema
    {
    private:
        /// Used to determine how GUI elements are positioned.
        std::stack<Vector2> layoutStack;

        /// Used to determine whether elements should be positioned vertically or horizontally.
        std::stack<bool> layoutDirection;

        /// Used to determine how much to move after completing a horizontal or vertical layout group.
        std::stack<float> layoutDifference;

        // Store handles to the corresponding bindless functions.
        int mouseActionId;
        int keyboardActionId;
        int textActionId;

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
        struct EditorInputState : public Schema<EditorInputState, 7>
        {
            DECLARE_BASE_SCHEMA(EditorInputState, 7);

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
        EditorInputState& InputState = input_state;

    public:
        CONSTRUCT_SCHEMA(SchemaRoot, EditorViewportSchema);

        EditorGUI() = default;
        virtual ~EditorGUI();

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

        /// When in a BeginHorizontal() or BeginVertical() block, returns the size difference between the beginning and end of the block so far.
        /// Note this is susceptible to change as you add more GUI elements or move further along the layout inside the block.
        float GetCurrentBlockSize();

        /// Displays some text.
        void TextLabel(std::string text);
        void TextLabel(std::string text, StyleText style);

        /// Takes text input and displays it. Returns the current string input.
        std::string TextField(std::string text);
        std::string TextField(std::string text, StyleClickable style, SDL_Color cursorColor = Colors::BLACK);

        /// Dumps an image at the current layout position.
        void PlaceImage(Image* image);

        /// Displays a button that takes input. When a user activates the button, this returns true.
        bool Button(std::string text, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool useMaxWidth = false, bool* isHovered = nullptr, bool* isPressed = nullptr);
        bool Button(std::string text, StyleClickable style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool useMaxWidth = false, bool* isHovered = nullptr, bool* isPressed = nullptr);
        bool Button(std::string text, TextLayout& textLayout, StyleClickable style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool useMaxWidth = false, bool* isHovered = nullptr, bool* isPressed = nullptr);
        bool Button(Image* image, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool* isHovered = nullptr, bool* isPressed = nullptr);
        bool Button(Image* image, StyleClickable style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, bool* isHovered = nullptr, bool* isPressed = nullptr);
        bool Button(int w, int h, StyleClickable style, bool invertOutline = true, Uint32 xpadding = 4, Uint32 ypadding = 4, Image* image = nullptr, bool* isHovered = nullptr, bool* isPressed = nullptr);

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
        bool Toggle(bool toggleValue, StyleClickable style, Vector2 boxSize, SDL_Color checkColor = Colors::BLACK);
        bool Toggle(bool toggleValue, StyleClickable style, Vector2 boxSize, std::function<void(bool)> drawFunc);

        float Slider(
             float sliderValue,
             float minValue = 0.0f,
             float maxValue = 1.0f,
             int length = 50,
             int buttonWidth = 8,
             int buttonHeight = 8,
             StyleClickable style = EditorStyle::EDITOR_SLIDER_STYLE,
             bool invertOutline = false,
             Uint32 xpadding = 4,
             Uint32 ypadding = 4
        );

        /// A drag-and-drop field that allows dragging and dropping of strings.
        /// This can be built upon to support SchemaReferable ID references or file paths.
        std::string DragAndDropField(std::string currentInput, bool* changed = nullptr);

        /// A drag-and-drop reference field for a specified implementation of SchemaReferable.
        /// A mapping function must be provided that converts a reference ID into a valid reference of the specified type.
        template<typename T>
        typename std::enable_if<std::is_base_of<SchemaReferable, T>::value, T*>::type
        ReferenceField(T* currentReference, std::function<T*(std::string)> findReference, std::string emptyField = "(null)")
        {
            if (IsVisible())
            {
                bool didChange = false;
                std::string result = DragAndDropField(
                    currentReference != nullptr ? currentReference->GetReferenceName() : emptyField,
                    &didChange
                );
                if (didChange)
                {
                    currentReference = findReference(result);
                }
            }
            return currentReference;
        }

        /// Displays a drop-down list of items when clicked. You must indicate which item is currently selected and provide a function to handle the dropdown selection.
        /// Optionally, provide a list of names corresponding to the given values. By default Utilities::ToString() is used  to determine these names.
        template<typename T>
        typename std::enable_if<!std::is_same<T, std::string>::value, void>::type
        Dropdown(unsigned int selected, const std::vector<T>& values, std::function<void(unsigned int)> onSelectHandler)
        {
            if (IsVisible())
            {
                Vector2 oldPos = GetLayoutPosition();
                if (Button(Utilities::ToString(values[selected])))
                {
                    std::vector<std::string> converted;
                    for (auto& value : values)
                    {
                        converted.push_back(Utilities::ToString(value));
                    }
                    OnDropdown(
                        Vector2(oldPos.x, GetLayoutPosition().y) + renderer->GetWindow()->GetPosition(),
                        converted,
                        onSelectHandler
                    );
                }
            }
        }

        void Dropdown(unsigned int selected, std::vector<std::string>& values, std::function<void(unsigned int)> onSelectHandler);

    private:
        void OnDropdown(Vector2 position, std::vector<std::string>& names, std::function<void(unsigned int)> onSelectHandler);

    };

}

#endif // EDITORGUI_H
