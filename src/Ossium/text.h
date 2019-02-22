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

        Text();
        Text(const Text& src);
        ~Text();

        friend class graphics::Renderer;

        /// Renders textData to a texture using a TrueType Font
        bool textToTexture(Renderer& renderer, Font* fontToUse, int pointSize = 0);

        /// Graphic override
        void Render(Renderer& renderer);

        /// Sets some of the more general text style properties
        void setStyling(int textStyle = TTF_STYLE_NORMAL, int textOutline = 0, int textHinting = TTF_HINTING_NORMAL, SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF});

        /// Oh boy, here come the get-set methods!
        /// I added these because if a member is set, the current text texture should be updated
        /// with the new text styling accordingly
        string getText();
        SDL_Color getColor();
        SDL_Color getBackgroundColor();

        /// Returns the dimensions of the text, or the dimensions of the box
        /// if the box is enabled
        int getWidth();
        int getHeight();

        int getOutline();
        int getStyle();
        int getHinting();
        int getRenderMode();
        int getBoxPaddingWidth();
        int getBoxPaddingHeight();
        bool isKerning();
        bool isBoxed();
        /// Set methods follow:
        void setText(string text);
        void setColor(SDL_Color textColor);
        void setBackgroundColor(SDL_Color textBackgroundColor);
        void setOutline(int textOutine);
        void setStyle(int textStyle);
        void setHinting(int textHinting);
        void setRenderMode(int textRenderMode);
        void setBoxPaddingWidth(int padWidth);
        void setBoxPaddingHeight(int padHeight);
        void setKerning(bool enabled);
        void setBox(bool enabled);

    private:
        /// If true, render box behind text in the background colour
        bool box;

        /// Box padding, in pixels
        int boxPadWidth;
        int boxPadHeight;

        /// If true, next time render() is called, the text is rerendered to texture first with textToTexture()
        bool update;

        /// The text string
        string textData;

        /// The image that is used to generate the text
        Image image;

        /// Pointer to font
        Font* font;

        /// Cached point size of the font
        int cachedPointSize;

        /// GUID of font to use:
        string font_guid;

        /// Text colour
        SDL_Color color;

        /// Background colour for outlines or shaded rendering box
        SDL_Color bgColor;

        /// Text outline thickness in pixels
        int outline;

        /// General text style, e.g. bold, italics etc.
        int style;

        /// Whether or not kerning should be enabled
        bool kerning;

        /// Font hinting, e.g. monospace
        int hinting;

        /// Solid, shaded or blended (alpha)
        int renderMode;

    };

}

#endif // TEXT_H
