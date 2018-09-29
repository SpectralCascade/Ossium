#ifndef OSS_TEXT_H
#define OSS_TEXT_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "oss_font.h"

using namespace std;

enum TextRenderModes
{
    OSS_RENDERTEXT_SOLID = 0,
    OSS_RENDERTEXT_SHADED,
    OSS_RENDERTEXT_BLEND
};

class OSS_Text
{
public:
    OSS_Text();
    ~OSS_Text();

    /// These two methods may be removed/replaced at some point, as OSS_Text is really an entity, not a resource
    bool load(string guid_path);
    bool init(string guid_path);

    /// Renders textData to a texture using a TrueType Font
    bool textToTexture(SDL_Renderer* renderer, OSS_Font* fontToUse);

    /// Renders the texture in a similar way to OSS_Texture::render()
    void render(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /// Simplified overload
    void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /// Very simple render method
    void renderSimple(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);

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

    /// The texture the text uses
    SDL_Texture* texture;

    /// The outline texture for the text
    SDL_Texture* outlineTexture;

    /// Pointer to font
    OSS_Font* font;

    /// GUID of font to use:
    string font_guid;

    /// Text colour
    SDL_Color color;

    /// Background colour for outlines or shaded rendering box
    SDL_Color bgColor;

    /// Text texture dimensions
    int width;
    int height;

    /// Outline texture dimensions
    int outlineWidth;
    int outlineHeight;

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

#endif // OSS_TEXT_H
