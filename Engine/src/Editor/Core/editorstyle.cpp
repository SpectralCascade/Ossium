#include "editorstyle.h"
#include "editorgui.h"

using namespace Ossium;

namespace Ossium::Editor
{

    EditorTheme::EditorTheme()
    {
        standardButton = EditorStyle::StandardButton;
    }

    namespace EditorStyle
    {
        StyleText StandardText = StyleText(
            "assets/Orkney Regular.ttf",
            12,
            Colors::Black,
            0,
            0,
            0,
            0,
            Typographic::TextAlignment::LEFT_ALIGNED,
            RENDERTEXT_BLEND_WRAPPED
        );
        StyleText StandardTextWhite = StyleText(
            "assets/Orkney Regular.ttf",
            12,
            Colors::White,
            0,
            0,
            0,
            0,
            Typographic::TextAlignment::LEFT_ALIGNED,
            RENDERTEXT_BLEND_WRAPPED
        );
        StyleText EDITOR_TEXT_GRAY_STYLE = StyleText(
            "assets/Orkney Regular.ttf",
            12,
            Color(180, 180, 180, 255),
            0,
            0,
            0,
            0,
            Typographic::TextAlignment::LEFT_ALIGNED,
            RENDERTEXT_BLEND_WRAPPED
        );
        StyleText EDITOR_TEXT_NORMAL_CENTERED_STYLE = StyleText(
            "assets/Orkney Regular.ttf",
            12,
            Colors::Black,
            0,
            0,
            0,
            0,
            Typographic::TextAlignment::CENTERED,
            RENDERTEXT_BLEND_WRAPPED
        );
        StyleText EDITOR_TEXT_INVERSE_CENTERED_STYLE = StyleText(
            "assets/Orkney Regular.ttf",
            12,
            Colors::White,
            0,
            0,
            0,
            0,
            Typographic::TextAlignment::CENTERED,
            RENDERTEXT_BLEND_WRAPPED
        );
        StyleClickable StandardButton = StyleClickable(
            Color(0, 255, 255),
            StandardText
        );
        StyleClickable EDITOR_SLIDER_STYLE = StyleClickable(
            Color(0, 255, 255),
            StandardText
        );
        StyleClickable EDITOR_SCROLLBAR_STYLE = StyleClickable(
            Color(100, 200, 200),
            StandardText
        );
        StyleClickable EDITOR_TEXTFIELD_STYLE = StyleClickable(
            Color(205, 205, 205) - 40,
            Color(205, 205, 205),
            Color(205, 205, 205),
            StandardText,
            StandardText,
            StandardText,
            Colors::Black,
            Colors::White,
            Colors::Black,
            Colors::White
        );
        StyleClickable EDITOR_DROPDOWN_ITEM_STYLE = StyleClickable(
            Color(0, 220, 220),
            Color(0, 200, 0),
            Color(0, 200, 0),
            StandardText,
            StandardTextWhite,
            StandardTextWhite,
            Colors::Transparent,
            Colors::Transparent,
            Colors::Transparent,
            Colors::Transparent
        );
        StyleClickable EDITOR_CHECKBOX_STYLE = StyleClickable(
            Colors::White,
            StandardText,
            Colors::Black,
            Colors::White,
            Colors::Black,
            Colors::White
        );
        StyleClickable EDITOR_CONTEXT_OPTION_STYLE = StyleClickable(
            Color(240, 240, 240),
            Color(0, 200, 255),
            Color(0, 200, 255),
            StandardText,
            StandardText,
            StandardText,
            Color(240, 240, 240),
            Color(240, 240, 240),
            Color(240, 240, 240),
            Color(240, 240, 240)
        );
        StyleClickable EDITOR_CONTEXT_OPTION_DISABLED_STYLE = StyleClickable(
            Color(240, 240, 240),
            Color(0, 200, 255),
            Color(0, 200, 255),
            EDITOR_TEXT_GRAY_STYLE,
            EDITOR_TEXT_GRAY_STYLE,
            EDITOR_TEXT_GRAY_STYLE,
            Color(240, 240, 240),
            Color(240, 240, 240),
            Color(240, 240, 240),
            Color(240, 240, 240)
        );
        StyleClickable ToolBarButton = StyleClickable(
            Color(250, 250, 250),
            Color(100, 210, 250),
            Color(100, 210, 250),
            StandardText,
            StandardText,
            StandardText,
            Color(250, 250, 250),
            Color(250, 250, 250),
            Color(250, 250, 250),
            Color(250, 250, 250)
        );
        StyleClickable HierarchyScene = StyleClickable(
            Color(210, 210, 210),
            Color(230, 230, 230),
            Color(110, 110, 110),
            StandardText,
            StandardText,
            StandardTextWhite,
            Color(40, 40, 40),
            Color(40, 40, 40),
            Color(40, 40, 40),
            Color(40, 40, 40)
        );
        StyleClickable HierarchySceneSelected = StyleClickable(
            Color(110, 110, 110),
            Color(110, 110, 110),
            Color(110, 110, 110),
            StandardTextWhite,
            StandardTextWhite,
            StandardTextWhite,
            Color(150, 150, 150),
            Color(150, 150, 150),
            Color(150, 150, 150),
            Color(150, 150, 150)
        );
        StyleClickable HierarchyEntity = StyleClickable(
            Color(210, 210, 210),
            Color(230, 230, 230),
            Color(180, 180, 180),
            StandardText,
            StandardText,
            StandardTextWhite,
            Color(40, 40, 40),
            Color(40, 40, 40),
            Color(40, 40, 40),
            Color(40, 40, 40)
        );
        StyleClickable HierarchyEntitySelected = StyleClickable(
            Color(180, 180, 180),
            Color(180, 180, 180),
            Color(180, 180, 180),
            StandardTextWhite,
            StandardTextWhite,
            StandardTextWhite,
            Color(110, 110, 110),
            Color(180, 180, 180),
            Color(180, 180, 180),
            Color(180, 180, 180)
        );
    }

    //
    // StyleClickable
    //

    StyleClickable::StyleClickable(SDL_Color bodyColor, StyleText textStyle)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalStyleText = textStyle;
        hoverStyleText = textStyle;
        clickStyleText = textStyle;
        bottomEdgeColor = Colors::Black;
        rightEdgeColor = Colors::Black;
        topEdgeColor = Colors::White;
        leftEdgeColor = Colors::White;
    }

    StyleClickable::StyleClickable(SDL_Color bodyColor, StyleText textStyle, SDL_Color outlineColor)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalStyleText = textStyle;
        hoverStyleText = textStyle;
        clickStyleText = textStyle;
        bottomEdgeColor = outlineColor;
        rightEdgeColor = outlineColor;
        topEdgeColor = outlineColor;
        leftEdgeColor = outlineColor;
    }

    StyleClickable::StyleClickable(
        SDL_Color bodyColor, StyleText textStyle, SDL_Color endEdgeColors, SDL_Color sideEdgeColors)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalStyleText = textStyle;
        hoverStyleText = textStyle;
        clickStyleText = textStyle;
        bottomEdgeColor = endEdgeColors;
        rightEdgeColor = sideEdgeColors;
        topEdgeColor = endEdgeColors;
        leftEdgeColor = sideEdgeColors;
    }

    StyleClickable::StyleClickable(
        SDL_Color bodyColor,
        StyleText textStyle,
        SDL_Color topColor,
        SDL_Color bottomColor,
        SDL_Color leftColor,
        SDL_Color rightColor)
    {
        normalColor = bodyColor - 35;
        hoverColor = bodyColor;
        clickColor = normalColor - 40;
        normalStyleText = textStyle;
        hoverStyleText = textStyle;
        clickStyleText = textStyle;
        bottomEdgeColor = bottomColor;
        rightEdgeColor = rightColor;
        topEdgeColor = topColor;
        leftEdgeColor = leftColor;
    }

    StyleClickable::StyleClickable(
        SDL_Color bodyNormal,
        SDL_Color bodyHover,
        SDL_Color bodyClick,
        StyleText textNormal,
        StyleText textHover,
        StyleText textClick,
        SDL_Color topColor,
        SDL_Color bottomColor,
        SDL_Color leftColor,
        SDL_Color rightColor)
    {
        normalColor = bodyNormal;
        hoverColor = bodyHover;
        clickColor = bodyClick;
        normalStyleText = textNormal;
        hoverStyleText = textHover;
        clickStyleText = textClick;
        bottomEdgeColor = bottomColor;
        rightEdgeColor = rightColor;
        topEdgeColor = topColor;
        leftEdgeColor = leftColor;
    }

}
