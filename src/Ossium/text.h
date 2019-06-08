#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "font.h"
#include "renderer.h"
#include "ecs.h"
#include "primitives.h"
#include "texture.h"
#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    class Text : public Texture
    {
    public:
        DECLARE_COMPONENT(Text);

        friend class graphics::Renderer;

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
