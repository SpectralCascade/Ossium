#ifndef TEXTLAYOUT_H
#define TEXTLAYOUT_H

#include <stack>

#include "font.h"
#include "schemamodel.h"

using namespace std;

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

    struct TextLayoutSchema : public Schema<TextLayoutSchema, 7>
    {
        DECLARE_BASE_SCHEMA(TextLayoutSchema, 7);

        /// Text alignment when rendered
        M(Typographic::TextAlignment, alignment) = Typographic::TextAlignment::LEFT_ALIGNED;

        /// Should the text be wrapped if it exceeds the bounding box when rendered?
        M(bool, lineWrap) = true;

        /// Should white space be ignored when line-wrapping?
        M(bool, ignoreWhitespace) = true;

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

    /// Internal helper struct for TextLines which stores information about a particular segment within a text line.
    struct TextLineSegment
    {
        Uint32 index;
        Uint8 style;
        SDL_Color color;
    };

    /// Helper class for laying out a single line of text.
    class TextLine
    {
    public:
        TextLine(float originalPointSize, float pointSize, SDL_Color startColor, Uint8 startStyle, Vector2 invalidGlyphDimensions);

        /// Adds a glyph to the current line segment.
        void AddGlyph(Glyph* glyph);

        /// Begins a new segment. Use this whenever you need to insert glyphs that have a different style or colour to the current segment.
        void BeginSegment(Glyph* glyph, Uint8 style, SDL_Color color);

        /// Returns the advance-based width of the line, scaled according to the point size of the glyphs. Does not account for kerning.
        float GetWidth();

        /// Returns the width of the line accounting for the bounding box of the final glyph instead of it's advance.
        float GetRenderedWidth();

        /// Returns the last segment begun.
        TextLineSegment GetCurrentSegment();

        /// Returns all glyphs on the line.
        const vector<Glyph*>& GetGlyphs();

        /// Returns all segments.
        const vector<TextLineSegment>& GetSegments();

    private:
        /// Sub-sections of the line that have different styles.
        vector<TextLineSegment> segments;

        /// The raw width of this line (the sum of all glyph advance widths).
        float width = 0;

        /// All glyphs on this line in sequential order.
        vector<Glyph*> glyphs;

        /// Used while computing width.
        float glyphScale;

        /// Used for null glyph dimensions.
        Vector2 invalidDimensions;

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
        void Render(Renderer& renderer, string text, Font& font, float pointSize, Rect boundingBox, SDL_Color mainColor = Colors::BLACK, int mainStyle = TTF_STYLE_NORMAL, bool applyMarkup = true, string lineBreakCharacters = " /!?|");

        /// Renders a line of glyphs.
        void RenderLine(Renderer& renderer, TextLine& line, Vector2 position, float pointSize, Font& font);

    private:
        /// Attempts to parse a tag. Returns false on invalid tag.
        bool ParseTag(string tagText, Uint32& boldTags, Uint32& italicTags, Uint32& underlineTags, Uint32& strikeTags, stack<SDL_Color>& colors, Uint8& style);

    };

}

#endif // TEXTLAYOUT_H
