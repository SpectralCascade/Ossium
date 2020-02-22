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

    struct TextLayoutSchema : public Schema<TextLayoutSchema, 9>
    {
        DECLARE_BASE_SCHEMA(TextLayoutSchema, 9);

        /// The default colour of the text.
        M(SDL_Color, mainColor) = Colors::BLACK;

        /// The default style of the text.
        M(int, mainStyle) = TTF_STYLE_NORMAL;

    protected:
        /// Size of the text.
        M(float, pointSize);

        /// Text alignment when rendered
        M(Typographic::TextAlignment, alignment) = Typographic::TextAlignment::LEFT_ALIGNED;

        /// The direction of the text; differs between languages.
        M(Typographic::TextDirection, direction) = Typographic::TextDirection::LEFT_TO_RIGHT;

        /// Should the text be wrapped if it exceeds the bounding box when rendered?
        M(bool, lineWrap) = true;

        /// Should white space be ignored when line-wrapping?
        M(bool, ignoreWhitespace) = true;

        /// Should words be broken if they're too long and exceed the bounding box? Only applicable when line wrapping.
        M(bool, wordBreak) = false;

        /// Should kerning be applied to the text? Note this does not always work with modern fonts at the time of writing.
        /// (requires Harfbuzz or similar text shaping library to do the work).
        M(bool, kerning) = true;

    };

    // Forward declarations
    class Font;

    /// Helper struct for TextLines which stores information about a particular segment within a text line.
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

        /// Adds a glyph to the line.
        void AddGlyph(GlyphMeta glyph);

        /// Removes a glyph from the end of the line.
        GlyphMeta PopGlyph();

        /// Removes all whitespace glyphs from the end of the line.
        void PopWhitespace();

        /// Begins a new segment. Use this whenever you need to insert glyphs that have a different style or colour to the current segment.
        void BeginSegment(GlyphMeta glyph, Uint8 style, SDL_Color color);

        /// Returns the advance-based width of the line, scaled according to the point size of the glyphs. Does not account for kerning.
        float GetWidth();

        /// Returns the width of the line accounting for the bounding box of the final glyph instead of it's advance.
        float GetRenderedWidth();

        /// Returns all glyphs on the line.
        const vector<GlyphMeta>& GetGlyphs();

        /// Returns all segments.
        const vector<TextLineSegment>& GetSegments();

        /// Clears all glyphs and line segments (except for the original segment).
        void Clear(bool resetWidth = true);

        /// Returns a new line with the glyphs and segments copied over from the specified index onwards. Removes any leftover white space from the end of this line.
        TextLine GetNewline(Uint32 lineBreakIndex, Uint32 lineSegmentBreakIndex, float originalPointSize, float pointSize, Vector2 invalidGlyphDimensions, bool removeWhitespace = true);

        /// The start position of this line.
        Vector2 position;

    private:
        /// Sub-sections of the line that have different styles.
        vector<TextLineSegment> segments;

        /// The raw width of this line (the sum of all glyph advance widths).
        float width = 0;

        /// All glyphs on this line in sequential order.
        vector<GlyphMeta> glyphs;

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

        /// Renders the text in the current layout.
        void Render(Renderer& renderer, Font& font, Vector2 position);

        /// Sets the bounding box. Note that this method triggers computation of layout on the next Update() or Render() method call.
        void SetBounds(Vector2 bounds);

        /// Returns the bounding box.
        Vector2 GetBounds();

        /// Sets the text string to be used. If the string is different to the cached string, also triggers layout computation on next Update() or Render().
        /**
            Given a text string with <i>these tags</i> will produce italic text, while <b>these tags</b> will produce bold text.
            You can also specify coloured text with <color=#FF0000FF>these tags</color> where #FF0000FF can be replaced with a hexadecimal colour code
            of your choosing, in the format #RRGGBBAA (red, red, green, green, blue, blue, alpha, alpha), or alternatively #RRGGBB.
            If you want to ignore tags in some parts of your text, you can use backslash \ before angle brackets < or >. Use double-backslash to show a single, literal backslash.
            Alternatively, you can disable these features entirely by passing 'false' as the applyMarkup argument.
            Also takes natural line break characters (ASCII only) that are used for line wrapping without breaking words.
        */
        void SetText(string str);

        /// Returns the text string currently used to compute layout.
        string GetText();

        /// If the layout has been modified, this method will recompute the layout.
        void Update(Renderer& renderer, Font& font);

        /// Returns the dimensions of the text layout.
        Vector2 GetSize();

        /// Returns the text alignment mode
        Typographic::TextAlignment GetAlignment();

        /// Returns the text direction
        Typographic::TextDirection GetDirection();

        /// Returns true if kerning is applied to text.
        bool IsKerning();

        /// Returns true if line wrapping is applied.
        bool IsLineWrapping();

        /// Returns true if line wrapping can break words.
        bool IsWordBreaking();

        /// Returns true if end line white-space is ignored when line wrapping.
        bool IsIgnoringWhitespace();

        /// Returns the point size.
        float GetPointSize();

        // Associated setters, should be self explanatory.
        void SetAlignment(Typographic::TextAlignment alignMode);
        void SetDirection(Typographic::TextDirection textDirection);
        void SetKerning(bool kern);
        void SetLineWrapping(bool wrap);
        void SetWordBreaking(bool midwordBreak);
        void SetIgnoringWhitespace(bool ignoreSpaces);
        void SetPointSize(float ptSize);

    private:
        /// Attempts to parse a tag. Returns false on invalid tag.
        bool ParseTag(string tagText, Uint32& boldTags, Uint32& italicTags, Uint32& underlineTags, Uint32& strikeTags, stack<SDL_Color>& colors, Uint8& style);

        /// Computes the text layout, parses tags and batch packs as many glyphs from the text string as possible.
        void ComputeLayout(Renderer& renderer, Font& font, bool applyMarkup = true, string lineBreakCharacters = " /!?|");

        /// Computes the positions of each line.
        void ComputeLinePositions(Font& font);

        /// Renders a single line of glyphs.
        void RenderLine(Renderer& renderer, Vector2 position, TextLine& line, Font& font);

        /// All lines of glyphs.
        vector<TextLine> lines;

        /// The bounding box dimensions of this text layout.
        Vector2 bbox;

        /// The text to layout and render.
        string text;

        /// Flag that indicates whether the positioning of lines should be updated.
        bool updateLines = true;

        /// Flag that indicates whether the layout should be recomputed.
        bool updateAll = true;

        /// The dimensions of the computed text layout.
        Vector2 size = Vector2::Zero;

    };

}

#endif // TEXTLAYOUT_H
