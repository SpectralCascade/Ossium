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
#include <stack>

#include "utf8.h"
#include "textlayout.h"

using namespace std;

namespace Ossium
{

    //
    // TextLine
    //

    TextLine::TextLine(float originalPointSize, float pointSize, SDL_Color startColor, Uint8 startStyle, Vector2 invalidGlyphDimensions)
    {
        segments.push_back((TextLineSegment){0, startStyle, startColor});
        glyphScale = pointSize / originalPointSize;
        invalidDimensions = invalidGlyphDimensions;
    }

    void TextLine::AddGlyph(GlyphMeta glyph)
    {
        glyphs.push_back(glyph);
        width += glyph.GetCodepoint() != 0 ? (float)glyph.GetAdvance() : invalidDimensions.x;
    }

    GlyphMeta TextLine::PopGlyph()
    {
        GlyphMeta glyph;
        if (!glyphs.empty())
        {
            glyph = glyphs.back();
            glyphs.pop_back();
            width -= glyph.GetCodepoint() != 0 ? (float)glyph.GetAdvance() : invalidDimensions.x;
        }
        return glyph;
    }

    void TextLine::PopWhitespace()
    {
        while (!glyphs.empty() && glyphs.back().GetCodepoint() == 32)
        {
            PopGlyph();
        }
    }

    void TextLine::BeginSegment(GlyphMeta glyph, Uint8 style, SDL_Color color)
    {
        AddGlyph(glyph);
        segments.push_back((TextLineSegment){segments.size(), style, color});
    }

    float TextLine::GetWidth()
    {
        return width * glyphScale;
    }

    float TextLine::GetRenderedWidth()
    {
        if (glyphs.empty())
        {
            return 0;
        }
        else if (glyphs.back().GetCodepoint() == 0)
        {
            return GetWidth();
        }
        return ((width - (float)glyphs.back().GetAdvance()) + ((float)glyphs.back().GetDimensions().x)) * glyphScale;
    }

    TextLineSegment TextLine::GetCurrentSegment()
    {
        return segments.back();
    }

    const vector<GlyphMeta>& TextLine::GetGlyphs()
    {
        return glyphs;
    }

    const vector<TextLineSegment>& TextLine::GetSegments()
    {
        return segments;
    }

    void TextLine::Clear(bool resetWidth)
    {
        glyphs.clear();
        TextLineSegment original = segments[0];
        segments.clear();
        segments.push_back(original);
        if (resetWidth)
        {
            width = 0;
        }
    }

    TextLine TextLine::GetNewline(Uint32 lineBreakIndex, Uint32 lineSegmentBreakIndex, float originalPointSize, float pointSize, Vector2 invalidGlyphDimensions, bool removeWhitespace)
    {
        if (lineBreakIndex >= glyphs.size() || lineSegmentBreakIndex >= segments.size())
        {
            Logger::EngineLog().Warning("Failed to get new TextLine for line break index {0} [segment break index {1}]!", lineBreakIndex, lineSegmentBreakIndex);
            return TextLine(originalPointSize, pointSize, segments[0].color, segments[0].style, invalidGlyphDimensions);
        }
        TextLineSegment& segment = segments[lineSegmentBreakIndex];

        if (segments[lineSegmentBreakIndex].index == lineBreakIndex + 1)
        {
            Logger::EngineLog().Debug("Removing line segment index {0}", lineSegmentBreakIndex);
            segments.erase(segments.begin() + lineSegmentBreakIndex);
        }

        Uint32 totalSegments = segments.size();

        TextLine nextLine = TextLine(originalPointSize, pointSize, segment.color, segment.style, invalidGlyphDimensions);

        string debugOut;

        // Move everything after the line break index into the new line
        for (Uint32 segmentIndex = lineSegmentBreakIndex; segmentIndex < totalSegments; segmentIndex++)
        {
            // Get start index, according to segment or original line break character
            Uint32 glyphIndex = max(segment.index, lineBreakIndex + 1);

            // Now add the glyphs to the new line until we reach the next segment
            for (Uint32 countUp = glyphIndex, nextSegment = (lineSegmentBreakIndex < segments.size() - 1 ? segments[lineSegmentBreakIndex + 1].index : glyphs.size());
                 countUp < nextSegment; countUp++)
            {
                nextLine.AddGlyph(glyphs[glyphIndex]);
                // Remove the glyph pointer we just copied
                glyphs.erase(glyphs.begin() + glyphIndex);
                debugOut += (char)nextLine.glyphs.back().GetCodepoint();
            }

            if (lineSegmentBreakIndex < segments.size() - 1)
            {
                // Remove the segment from this line
                segments.erase(segments.begin() + lineSegmentBreakIndex + 1);
                // Copy the segment to the new line along with the first glyph
                nextLine.BeginSegment(glyphs[glyphIndex], segment.style, segment.color);
                debugOut += (char)nextLine.glyphs.back().GetCodepoint();
                // Remove the first glyph pointer we just copied
                glyphs.erase(glyphs.begin() + glyphIndex);
                // Get the current segment
                segment = segments[lineSegmentBreakIndex];
            }
        }
        width -= nextLine.width;

        if (removeWhitespace)
        {
            PopWhitespace();
        }

        return nextLine;
    }

    //
    // TextLayout
    //

    void TextLayout::Render(Renderer& renderer, Font& font, Vector2 position)
    {
        // Update just in case anything has changed
        Update(renderer, font);

        for (auto line : lines)
        {
            RenderLine(renderer, position, line, font);
        }
    }

    void TextLayout::RenderLine(Renderer& renderer, Vector2 position, TextLine& line, Font& font)
    {
        if (line.GetGlyphs().empty())
        {
            // Early out
            return;
        }

        position += line.position;
        Vector2 startPos = position;

        for (Uint32 i = 0, counti = line.GetSegments().size(); i < counti; i++)
        {
            Uint32 nextSegment = i + 1 < line.GetSegments().size() ? line.GetSegments()[i + 1].index : line.GetGlyphs().size();
            for (Uint32 index = line.GetSegments()[i].index; index < nextSegment; index++)
            {
                GlyphMeta meta = line.GetGlyphs()[index];
                GlyphID glyph = CreateGlyphID(meta.GetCodepoint(), line.GetSegments()[i].style, 0, 0);
                font.RenderGlyph(
                    renderer,
                    glyph,
                    position,
                    pointSize,
                    line.GetSegments()[i].color,
                    kerning,
                    direction
                );
                position.x += direction == Typographic::TextDirection::LEFT_TO_RIGHT ? meta.GetAdvance(pointSize) : -meta.GetAdvance(pointSize);
            }
            if (line.GetSegments()[i].style & TTF_STYLE_UNDERLINE)
            {
                Vector2 underlinePos = Vector2(0, font.GetUnderlinePosition(pointSize));
                Line underline(startPos + underlinePos, position + underlinePos);
                underline.Draw(renderer, line.GetSegments()[i].color);
            }
            if (line.GetSegments()[i].style & TTF_STYLE_STRIKETHROUGH)
            {
                Vector2 strikethroughPos = Vector2(0, font.GetStrikethroughPosition(pointSize));
                Line strikethrough(startPos + strikethroughPos, position + strikethroughPos);
                strikethrough.Draw(renderer, line.GetSegments()[i].color);
            }
        }

    }

    void TextLayout::ComputeLayout(Renderer& renderer, Font& font, bool applyMarkup, string lineBreakCharacters)
    {
        bool isTag = false;
        bool isEscaped = false;
        string tagText;
        Uint32 boldTags = mainStyle & TTF_STYLE_BOLD ? 1 : 0;
        Uint32 italicTags = mainStyle & TTF_STYLE_ITALIC ? 1 : 0;
        Uint32 underlineTags = mainStyle & TTF_STYLE_UNDERLINE ? 1 : 0;
        Uint32 strikeTags = mainStyle & TTF_STYLE_STRIKETHROUGH ? 1 : 0;
        stack<SDL_Color> colours;
        colours.push(mainColor);
        Uint8 style = (Uint8)(mainStyle & (TTF_STYLE_UNDERLINE | TTF_STYLE_STRIKETHROUGH));

        lines.clear();
        lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));

        if (text.empty())
        {
            // Early out
            return;
        }

        // Keeps track of the last ideal place in the line to make a line break
        Uint32 lineBreakIndex = 0;
        // Counts line segment indices
        Uint32 lineSegmentCounter = 0;

        font.BatchPackBegin(renderer);
        for (unsigned int i = 0, counti = text.length(); i < counti;)
        {
            // Extract UTF-8 character
            Uint8 bytes = (Uint8)max((int)Utilities::CheckUTF8(text[i]), 1);
            string utfChar = text.substr(i, bytes);
            i += bytes;

            bool addLineSegment = false;
            bool wasTag = isTag;
            if (applyMarkup)
            {
                // Do tag parsing
                if (isEscaped)
                {
                    isEscaped = false;
                }
                else if (bytes <= 1)
                {
                    char c = utfChar[0];
                    // Check for escape character and tag indicators
                    if (c == '\\')
                    {
                        isEscaped = true;
                        continue;
                    }
                    else if (c == '<')
                    {
                        isTag = true;
                    }
                    else if (c == '>' && isTag)
                    {
                        isTag = false;
                        // Now parse the tag itself.
                        if (!tagText.empty())
                        {
                            Uint32 oldUnderlineTags = underlineTags;
                            Uint32 oldStrikeTags = strikeTags;
                            if (!ParseTag(tagText, boldTags, italicTags, underlineTags, strikeTags, colours, style))
                            {
                                Logger::EngineLog().Warning("Failed to parse tag '<{0}>' in string '{1}'.", tagText, text);
                            }
                            if (mainColor != colours.top())
                            {
                                mainColor = colours.top();
                                addLineSegment = true;
                            }
                            else if (underlineTags != oldUnderlineTags || strikeTags != oldStrikeTags)
                            {
                                addLineSegment = true;
                            }
                            tagText.clear();
                        }
                    }
                    else if (isTag)
                    {
                        tagText += c;
                    }
                }
            }

            // Exclude tags and non-printable ASCII characters
            if (!isTag && !wasTag && !(bytes <= 1 && (utfChar[0] < 32 || utfChar[0] == 127)))
            {
                // Pack the glyph.
                GlyphMeta glyph = font.GetGlyphMeta(Utilities::GetCodepointUTF8(utfChar));

                bool lineChange = false;

                // If there is more than one glyph on the line and the line exceeds the bounding box, wrap to a new line.
                if (lineWrap && ((lines.back().GetGlyphs().size() > 0) && (!(ignoreWhitespace && utfChar[0] == ' ')) &&
                    (lines.back().GetWidth() + (glyph.GetCodepoint() != 0 ?
                        glyph.GetDimensions(pointSize).x : font.GetInvalidGlyphDimensions(pointSize).x) >= bbox.x
                    ))
                )
                {
                    lineChange = true;
                    // Should we break the line at the current glyph, or the last ideal break glyph?
                    if (wordBreak)
                    {
                        // Break mid-word (current glyph).
                        lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));
                    }
                    else if (lineBreakIndex > 0)
                    {
                        // Natural line break at end of a word or statement or whatever.
                        lines.push_back(lines.back().GetNewline(lineBreakIndex, lineSegmentCounter, font.GetLoadedPointSize(), pointSize, font.GetInvalidGlyphDimensions(pointSize)));
                    }
                    lineBreakIndex = 0;
                    lineSegmentCounter = 0;
                }

                // Add glyph to the current line
                if (addLineSegment && !lineChange)
                {
                    lines.back().BeginSegment(glyph, style, mainColor);
                    lineSegmentCounter++;
                }
                else
                {
                    lines.back().AddGlyph(glyph);
                }

                // Only pack while this batch has space in the font atlas.
                if (font.GetBatchPackTotal() < font.GetAtlasMaxGlyphs())
                {
                    // Batch pack glyphs now to save processing time during rendering.
                    font.BatchPackGlyph(renderer, CreateGlyphID(glyph.GetCodepoint(), style, 0, 0));
                }

                if (lineWrap)
                {
                    if (bytes <= 1 && !lines.back().GetGlyphs().empty())
                    {
                        for (auto c : lineBreakCharacters)
                        {
                            if (utfChar[0] == c)
                            {
                                lineBreakIndex = lines.back().GetGlyphs().size() - 1;
                                break;
                            }
                        }
                    }
                }

            }
            else if (utfChar[0] == '\n')
            {
                // Add newline
                lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));
                lineBreakIndex = 0;
                lineSegmentCounter = 0;
            }

        }
        font.BatchPackEnd(renderer);

        // Update line positioning
        ComputeLinePositions(font);

        updateAll = false;

    }

    void TextLayout::ComputeLinePositions(Font& font)
    {
        float lineY = 0;
        for (TextLine& line : lines)
        {
            switch (alignment)
            {
            case Typographic::TextAlignment::RIGHT_ALIGNED:
                line.position.x = bbox.x - ceil(line.GetRenderedWidth());
                break;
            case Typographic::TextAlignment::CENTERED:
                line.position.x = (bbox.x / 2) - ceil(line.GetRenderedWidth() / 2.0f);
                break;
            default:
                line.position.x = 0;
                break;
            }
            line.position.y = lineY;
            lineY += font.GetLineDifference(pointSize);
            size = size.Max(Vector2(line.GetRenderedWidth(), lineY));
        }
        updateLines = false;
    }

    void TextLayout::SetBounds(Vector2 bounds)
    {
        if (bbox != bounds)
        {
            bbox = bounds;
            updateAll = true;
        }
    }

    Vector2 TextLayout::GetBounds()
    {
        return bbox;
    }

    void TextLayout::SetText(string str)
    {
        if (str.empty() || str.size() != (text.empty() ? 0 : text.size()) || text != str)
        {
            text = str;
            updateAll = true;
        }
    }

    string TextLayout::GetText()
    {
        return text;
    }

    void TextLayout::Update(Renderer& renderer, Font& font)
    {
        if (updateAll)
        {
            ComputeLayout(renderer, font);
        }
        else if (updateLines)
        {
            ComputeLinePositions(font);
        }
    }

    Vector2 TextLayout::GetSize()
    {
        return size;
    }

    Typographic::TextAlignment TextLayout::GetAlignment()
    {
        return alignment;
    }

    Typographic::TextDirection TextLayout::GetDirection()
    {
        return direction;
    }

    bool TextLayout::IsKerning()
    {
        return kerning;
    }

    bool TextLayout::IsLineWrapping()
    {
        return lineWrap;
    }

    bool TextLayout::IsWordBreaking()
    {
        return wordBreak;
    }

    bool TextLayout::IsIgnoringWhitespace()
    {
        return ignoreWhitespace;
    }

    float TextLayout::GetPointSize()
    {
        return pointSize;
    }

    void TextLayout::SetAlignment(Typographic::TextAlignment alignMode)
    {
        if (alignment != alignMode)
        {
            alignment = alignMode;
            updateLines = true;
        }
    }

    void TextLayout::SetDirection(Typographic::TextDirection textDirection)
    {
        if (direction != textDirection)
        {
            direction = textDirection;
            updateAll = true;
        }
    }

    void TextLayout::SetKerning(bool kern)
    {
        if (kerning != kern)
        {
            kerning = kern;
            // TODO: update all if kerning affects layout in future
        }
    }

    void TextLayout::SetLineWrapping(bool wrap)
    {
        if (lineWrap != wrap)
        {
            lineWrap = wrap;
            updateAll = true;
        }
    }

    void TextLayout::SetWordBreaking(bool midwordBreak)
    {
        if (wordBreak != midwordBreak)
        {
            wordBreak = midwordBreak;
            updateAll = true;
        }
    }

    void TextLayout::SetIgnoringWhitespace(bool ignoreSpaces)
    {
        if (ignoreWhitespace != ignoreSpaces)
        {
            ignoreWhitespace = ignoreSpaces;
            updateAll = true;
        }
    }

    void TextLayout::SetPointSize(float ptSize)
    {
        if (pointSize != ptSize)
        {
            pointSize = ptSize;
            updateAll = true;
        }
    }

    bool TextLayout::ParseTag(string tagText, Uint32& boldTags, Uint32& italicTags, Uint32& underlineTags, Uint32& strikeTags, stack<SDL_Color>& colors, Uint8& style)
    {
        unsigned int tagTextLength = tagText.length();
        bool success = true;
        if (tagText[0] == '/' && tagTextLength > 1)
        {
            if (tagText[1] == 'b')
            {
                boldTags = max((Uint32)0, boldTags - 1);
            }
            else if (tagText[1] == 'i')
            {
                italicTags = max((Uint32)0, italicTags - 1);
            }
            else if (tagText[1] == 'u')
            {
                underlineTags = max((Uint32)0, underlineTags - 1);
                if (underlineTags == 0)
                {
                    style = style & ~TTF_STYLE_UNDERLINE;
                }
            }
            else if (tagText[1] == 's')
            {
                strikeTags = max((Uint32)0, strikeTags - 1);
                if (strikeTags == 0)
                {
                    style = style & ~TTF_STYLE_STRIKETHROUGH;
                }
            }
            else if (tagText == "/color" && colors.size() > 1)
            {
                colors.pop();
            }
            else
            {
                success = false;
            }
        }
        else
        {
            if (tagText[0] == 'b')
            {
                boldTags++;
            }
            else if (tagText[0] == 'i')
            {
                italicTags++;
            }
            else if (tagText[0] == 'u')
            {
                underlineTags++;
                style = style | TTF_STYLE_UNDERLINE;
            }
            else if (tagText[0] == 's')
            {
                strikeTags++;
                style = style | TTF_STYLE_STRIKETHROUGH;
            }
            else if (tagTextLength >= 13 && tagText.substr(0, 7) == "color=#")
            {
                // Get the actual colour
                tagText = Utilities::SplitRight(tagText, '#', "FF0000");
                tagTextLength = tagText.length();
                SDL_Color mainColor = colors.top();

                Uint32 converted = Utilities::ToUint32FromHex(tagText);
                switch (tagTextLength)
                {
                case 6:
                    converted = converted << 8;
                    converted |= 0x000000FF;
                case 8:
                    mainColor = Color((converted & 0xFF000000) >> 24, (converted & 0x00FF0000) >> 16, (converted & 0x0000FF00) >> 8, converted & 0x000000FF);
                    colors.push(mainColor);
                    break;
                default:
                    success = false;
                    break;
                }
            }
            else
            {
                success = false;
            }
        }
        return success;
    }

}
