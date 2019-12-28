/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "../Core/font.h"
#include "../Core/renderer.h"
#include "../Core/ecs.h"
#include "texture.h"
#include "../Core/helpermacros.h"

using namespace std;

namespace Ossium
{

    struct TextFormat : public Schema<TextFormat, 9>
    {
        DECLARE_BASE_SCHEMA(TextFormat, 9);

        TextFormat(
            string font = "",
            int fontSize = 12,
            SDL_Color color = Colors::BLACK,
            int hint = 0,
            int kern = 0,
            int outlineThickness = 0,
            int styling = 0,
            int renderingMode = RENDERTEXT_BLEND,
            SDL_Color backgroundColor = Colors::TRANSPARENT
        );

        M(string, fontPath);
        M(int, ptsize) = 12;
        M(SDL_Color, fg) = Colors::BLACK;
        M(int, hinting) = 0;
        M(int, kerning) = 0;
        M(int, outline) = 0;
        M(int, style) = 0;
        M(int, rendermode) = RENDERTEXT_SOLID;
        M(SDL_Color, bg) = Colors::TRANSPARENT;
    };

    class OSSIUM_EDL Text : public Texture
    {
    public:
        DECLARE_COMPONENT(Texture, Text);

        friend class Graphics::Renderer;

        virtual void OnClone();

        /// Renders textData to a texture using a TrueType Font
        bool TextToTexture(Renderer& renderer, Font* fontToUse, int pointSize = 0);

        /// Graphic override
        void Render(Renderer& renderer);

        /// Sets some of the more general text style properties
        void SetStyling(int textStyle = TTF_STYLE_NORMAL, int textOutline = 0, int textHinting = TTF_HINTING_NORMAL, SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF});

        /// Oh boy, here come the get-set methods!
        /// I added these because if a member is set, the current text texture should be updated
        /// with the new text styling accordingly
        string GetText();
        SDL_Color GetColor();
        SDL_Color GetBackgroundColor();

        /// Returns the dimensions of the text, or the dimensions of the box
        /// if the box is enabled
        int GetWidth();
        int GetHeight();

        int GetOutline();
        int GetStyle();
        int GetHinting();
        int GetRenderMode();
        int GetBoxPaddingWidth();
        int GetBoxPaddingHeight();
        bool IsKerning();
        bool IsBoxed();
        /// Set methods follow:
        void SetText(string text);
        void SetColor(SDL_Color textColor);
        void SetBackgroundColor(SDL_Color textBackgroundColor);
        void SetOutline(int textOutine);
        void SetStyle(int textStyle);
        void SetHinting(int textHinting);
        void SetRenderMode(int textRenderMode);
        void SetBoxPaddingWidth(int padWidth);
        void SetBoxPaddingHeight(int padHeight);
        void SetKerning(bool enabled);
        void SetBox(bool enabled);

    private:
        /// If true, render box behind text in the background color
        bool box = false;

        /// Box padding, in pixels
        int boxPadWidth = 4;
        int boxPadHeight = 2;

        /// If true, next time render() is called, the text is rerendered to texture first with TextToTexture()
        bool update = true;

        /// The text string
        string textData = "";

        /// The image that is used to generate the text
        Image image;

        /// Pointer to font
        Font* font = nullptr;

        /// Cached point size of the font
        int cachedPointSize = 24;

        /// GUID of font to use:
        string font_guid;

        /// Text color
        SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};

        /// Background color for outlines or shaded rendering box
        SDL_Color bgColor = {0x00, 0x00, 0x00, 0xFF};

        /// Text outline thickness in pixels
        int outline = 0;

        /// General text style, e.g. bold, italics etc.
        int style = TTF_STYLE_NORMAL;

        /// Whether or not kerning should be enabled
        bool kerning = true;

        /// Font hinting, e.g. monospace
        int hinting = TTF_HINTING_NORMAL;

        /// Solid, shaded or blended (alpha)
        int renderMode = RENDERTEXT_SOLID;

    };

}

#endif // TEXT_H
