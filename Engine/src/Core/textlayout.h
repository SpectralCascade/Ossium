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

    struct TextLayoutSchema : public Schema<TextLayoutSchema, 9>
    {
        DECLARE_BASE_SCHEMA(TextLayoutSchema, 9);

        /// The default colour of the text.
        M(SDL_Color, mainColor) = Colors::BLACK;

        /// The default style of the text.
        M(int, mainStyle) = TTF_STYLE_NORMAL;

    protected:
        /// Default size of the text.
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

    /// Helper struct for laying out a single line of text.
    struct TextLine
    {
        /// The relative position of this line compared to other lines in a text layout.
        Vector2 position;

        /// The dimensions of the line.
        Vector2 size;

        /// The index of the glyph at the start of this line.
        Uint32 glyphIndex;

    };

    /// Grouping of glyphs that have the same point size, color, style, hinting and outline.
    struct GlyphGroup
    {
        /// The start index of the group in the codepoint array.
        Uint32 index;

        /// The relative size of the glyphs.
        float pointSize;

        /// The color of the glyphs.
        SDL_Color color;

        /// The glyph style (e.g. bold).
        Uint8 style;

        /// The type of hinting (e.g. light).
        Uint8 hinting;

        /// The outline thickness of the glyphs, in pixels.
        Uint8 outline;

        /// The color of the outline.
        SDL_Color outlineColor;

    };

    /// Stores information about a particular glyph, such as it's position, line, meta, glyph group, etc.
    struct GlyphLocation
    {
        GlyphMeta glyph;
        TextLine line;
        Vector2 position;
        int index;
        bool valid;
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
        void Render(Renderer& renderer, Font& font, Vector2 startPos);

        /// Sets the bounding box. Note that this method triggers computation of layout on the next Update() or Render() method call.
        void SetBounds(Vector2 bounds);

        /// Returns the bounding box.
        Vector2 GetBounds();

        /// Sets the text string to be used. Parses all tags and gets the corresponding glyph data for each character.
        /**
            Given a text string with <i>these tags</i> will produce italic text, while <b>these tags</b> will produce bold text.
            You can also specify coloured text with <color=#FF0000FF>these tags</color> where #FF0000FF can be replaced with a hexadecimal colour code
            of your choosing, in the format #RRGGBBAA (R = red, G = green, B = blue, A = alpha), or alternatively #RRGGBB.
            If you want to ignore tags in some parts of your text, you can use backslash \ before angle brackets < or >. Use double-backslash to show a single, literal backslash.
            Alternatively, you can disable these features entirely by passing 'false' as the applyMarkup argument.
            Also takes natural line break characters (ASCII only) that are used for line wrapping without breaking words.
        */
        void SetText(Renderer& renderer, Font& font, string str, bool applyMarkup);

        /// Recomputes layout if any changes have been made. This must be called before rendering or glyph location.
        void Update(Font& font);

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

        /// Attempts to find the glyph nearest the given position in this text layout (relative to the start of the first line).
        /**
            If a glyph could not be located (e.g. empty glyphs array or no layout has been calculated yet) then the returned GlyphLocation has it's valid flag set to false.
            Make sure you check the valid flag before use, otherwise you'll get junk data.
            Also be warned that this can be a slow method as it has to step through lines to find the closest glyph;
            worst case is approximately O(n + m) where n = number of lines, m = number of glyphs on the closest line.
        */
        GlyphLocation LocateGlyph(Vector2 position);

        /// Overload that returns a glyph location according to it's index rather than it's physical position.
        GlyphLocation LocateGlyph(int index);

        /// Returns the total number of glyphs in this text layout.
        unsigned int GetTotalGlyphs();

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

        /// Computes the text layout and batch packs as many glyphs from the text string as possible.
        // Other common line break characters may include '/', '!', '?' and '|'. By default only white space is broken.
        void ComputeLayout(Renderer& renderer, Font& font, string& text, bool applyMarkup, string lineBreakCharacters = " ");

        /// Computes the text layout using the pre-existing glyphs array. Useful when the bounds change.
        void ComputeLayout(TextLine& startLine, string lineBreakCharacters = " ");

        /// Computes the position of the next glyph given a specific glyph, applying line wrapping etc.
        void ComputeGlyphPosition(unsigned int glyphIndex, TextLine& line, const GlyphGroup& group, float& lastWordWidth, int lastLineBreakIndex = -1);

        /// Sets the position of a given text line and returns total.
        void ComputeLinePosition(TextLine& line, Vector2& position);

        /// Computes the positions of each line.
        void ComputeLinePositions();

        /// The bounding box dimensions of this text layout.
        Vector2 bbox;

        /// Specifies the starting index for each line in the glyphs array, as well as the size of each line.
        vector<TextLine> lines;

        /// Groupings of glyphs that have the same style, size, color etc. in sequential order.
        vector<GlyphGroup> groups;

        /// Array of glyphs, corresponding to the text string.
        vector<GlyphMeta> glyphs;

        /// Determines which parts of the text layout should be updated.
        Uint8 updateFlags = 0;

        // Flags specifying which parts of the layout need updating.
        const Uint8 UPDATE_LINES    =   0x01;
        const Uint8 UPDATE_LAYOUT   =   0x02;
        const Uint8 UPDATE_ALL      =   UPDATE_LINES | UPDATE_LAYOUT;

        /// The dimensions of the computed text layout.
        Vector2 size = Vector2::Zero;

    };

}

#endif // TEXTLAYOUT_H
