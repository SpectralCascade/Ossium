#ifndef TEXTLAYOUT_H
#define TEXTLAYOUT_H

#include "font.h"
#include "schemamodel.h"

namespace Ossium
{

    namespace Typographic
    {

        enum TextAlignment
        {
            LEFT_ALIGNED = 0,
            RIGHT_ALIGNED,
            CENTERED,
            JUSTIFIED
        };

    }

    struct TextLayoutSchema : public Schema<TextLayoutSchema, 6>
    {
        DECLARE_BASE_SCHEMA(TextLayoutSchema, 6);

        /// Text alignment when rendered
        M(Typographic::TextAlignment, alignment) = Typographic::TextAlignment::LEFT_ALIGNED;

        /// Should the text be wrapped if it exceeds the bounding box when rendered?
        M(bool, lineWrap) = true;

        /// Should words be broken if they're too long and exceed the bounding box? Only applicable when line wrapping.
        M(bool, wordBreak) = false;

        /// The direction of the text; differs between languages.
        M(Typographic::TextDirection, direction) = Typographic::TextDirection::LEFT_TO_RIGHT;

        /// Should kerning be applied to the text? Note this does not always work with modern fonts at the time of writing.
        /// (requires Harfbuzz or similar text-layout library to do the work).
        M(bool, kerning) = true;

        /// The last string to be rendered. If this matches the input string again, we don't need to update the layout.
        /// TODO: implement
        M(string, cachedText);

    };

    // Forward declarations
    class Font;

    /// Internal helper structure for text layout and stylistic details.
    struct GlyphBatch
    {
        GlyphBatch() = default;
        GlyphBatch(Vector2 startPos, SDL_Color mainColor, int addStyle);

        /// The position the glyphs are rendered from.
        Vector2 position;

        /// The color the glyphs are rendered in.
        SDL_Color color;

        /// The additive style the glyphs are rendered in
        /// (i.e. normal, underline or strikethrough but NOT bold or italic as those styles are pre-rendered in the glyph).
        int additiveStyle;

        /// Glyphs to render, in sequential order.
        vector<Glyph*> glyphs;
    };

    /// Basic text layout renderer.
    /**
        Does not properly support some languages due to the complexity of layout variation between languages
        which would require much more code (directly using FreeType and a text-shaping library like HarfBuzz, plus a line-break library like ICU).
        Fine to use for *most* (but not necessarily all) European languages and possibly some other languages.
        Make sure you check all rendered text thoroughly when rendering different languages.
    */
    class TextLayout : public TextLayoutSchema
    {
    public:
        CONSTRUCT_SCHEMA(SchemaRoot, TextLayoutSchema);

        /// Renders some text within a specified bounding box. By default it supports some markup tags as described below.
        /**
            Given a text string with <i>these tags</i> will produce italic text, while <b>these tags</b> will produce bold text.
            You can also specify coloured text with <color=#FF0000FF>these tags</color> where #FF0000FF can be replaced with a hexadecimal colour code
            of your choosing, in the format #RRGGBBAA (red, red, green, green, blue, blue, alpha, alpha), or alternatively #RRGGBB.
            If you want to ignore tags in some parts of your text, you can use backslash \ before angle brackets < or >. Use double-backslash to show a single, literal backslash.
            Alternatively, you can disable these features entirely by passing 'false' as the applyMarkup argument.
        */
        void Render(Renderer& renderer, string text, Vector2 position, Font& font, float pointSize, Rect boundingBox, SDL_Color mainColor = Colors::BLACK, int mainStyle = TTF_STYLE_NORMAL, bool applyMarkup = true, string lineBreakCharacters = " /!?|");

    private:
        /// Attempts to parse a tag. Returns false on invalid tag.
        bool ParseTag(string tagText, Uint32& boldTags, Uint32& italicTags, Uint32& underlineTags, Uint32& strikeTags, stack<SDL_Color>& colors, int& style);

        /// Renders a batch of glyphs and returns the difference vector.
        Vector2 RenderBatch(Renderer& renderer, GlyphBatch& batch, float pointSize, Font& font);

    };

}

#endif // TEXTLAYOUT_H
