#ifndef EDITORSTYLE_H
#define EDITORSTYLE_H

#include "../../Core/schemamodel.h"
#include "../../Core/font.h"

using namespace Ossium;

namespace Ossium::Editor
{

    /// Defines the style of a clickable EditorGUI element.
    struct StyleClickable : public Schema<StyleClickable, 10>
    {
        DECLARE_BASE_SCHEMA(StyleClickable, 10);

        StyleClickable() = default;

        StyleClickable(
            SDL_Color bodyColor,
            StyleText textStyle
        );

        StyleClickable(
            SDL_Color bodyColor,
            StyleText textStyle,
            SDL_Color outlineColor
        );

        StyleClickable(
            SDL_Color bodyColor,
            StyleText textStyle,
            SDL_Color endEdgeColors,
            SDL_Color sideEdgeColors
        );

        StyleClickable(
            SDL_Color bodyColor,
            StyleText textStyle,
            SDL_Color topColor,
            SDL_Color bottomColor,
            SDL_Color leftColor,
            SDL_Color rightColor
        );

        StyleClickable(
            SDL_Color bodyNormal,
            SDL_Color bodyHover,
            SDL_Color bodyClick,
            StyleText textNormal,
            StyleText textHover,
            StyleText textClick,
            SDL_Color topColor,
            SDL_Color bottomColor,
            SDL_Color leftColor,
            SDL_Color rightColor
        );

        M(SDL_Color, normalColor) = Color(0, 220, 220);
        M(SDL_Color, hoverColor) = Color(0, 255, 255);
        M(SDL_Color, clickColor) = Color(0, 180, 180);

        M(StyleText, normalStyleText);
        M(StyleText, hoverStyleText);
        M(StyleText, clickStyleText);

        M(SDL_Color, topEdgeColor) = Colors::WHITE;
        M(SDL_Color, bottomEdgeColor) = Colors::BLACK;
        M(SDL_Color, leftEdgeColor) = Colors::WHITE;
        M(SDL_Color, rightEdgeColor) = Colors::BLACK;
    };

    namespace EditorStyle
    {
        extern StyleText StandardText;
        extern StyleText StandardTextWhite;
        extern StyleText EDITOR_TEXT_GRAY_STYLE;
        extern StyleText EDITOR_TEXT_NORMAL_CENTERED_STYLE;
        extern StyleText EDITOR_TEXT_INVERSE_CENTERED_STYLE;
        extern StyleClickable StandardButton;
        extern StyleClickable EDITOR_SLIDER_STYLE;
        extern StyleClickable EDITOR_SCROLLBAR_STYLE;
        extern StyleClickable EDITOR_DROPDOWN_ITEM_STYLE;
        extern StyleClickable EDITOR_TEXTFIELD_STYLE;
        extern StyleClickable EDITOR_CHECKBOX_STYLE;
        extern StyleClickable EDITOR_CONTEXT_OPTION_STYLE;
        extern StyleClickable EDITOR_CONTEXT_OPTION_DISABLED_STYLE;
        extern StyleClickable ToolBarButton;
        extern StyleClickable HierarchyScene;
        extern StyleClickable HierarchySceneSelected;
        extern StyleClickable HierarchyEntity;
        extern StyleClickable HierarchyEntitySelected;
    }

    /// A schema that defines how the editor should look.
    struct EditorTheme : Schema<EditorTheme, 50>
    {
        DECLARE_BASE_SCHEMA(EditorTheme, 50);

        /// Initialises with default styles.
        EditorTheme();

        /// A default style for generic buttons such as those in custom editor windows.
        M(StyleClickable, standardButton);

        /// A style for buttons in context menus/dropdown lists.
        M(StyleClickable, dropdownButton);

        /// A style for the main tool bar buttons.
        M(StyleClickable, toolbarButton);

    };

}

#endif // EDITORSTYLE_H
