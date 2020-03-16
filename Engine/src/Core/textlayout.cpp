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
    // TextLayout
    //

    void TextLayout::Render(Renderer& renderer, Font& font, Vector2 startPos)
    {
        GlyphGroup& currentGroup = groups.front();
        Vector2 linePos;
        // Iterate over each line
        for (unsigned int i = 0, group = 0, glyphIndex = 0, counti = lines.size(); i < counti; i++)
        {
            TextLine& line = lines[i];
            Vector2 position = startPos + line.position;

            //Logger::EngineLog().Info("Start pos = {0}, line pos = {1}, true linepos = {2}", startPos, line.position, position);
            linePos = position;

            unsigned int nextIndex = i + 1 < lines.size() ? lines[i + 1].glyphIndex : glyphs.size();
            while (glyphIndex < nextIndex)
            {
                if (currentGroup.outline != 0)
                {
                    // Render outline first
                    GlyphID glyph = CreateGlyphID(glyphs[glyphIndex].GetCodepoint(), currentGroup.style, currentGroup.hinting, currentGroup.outline);
                    font.RenderGlyph(
                        renderer,
                        glyph,
                        position,
                        currentGroup.pointSize,
                        currentGroup.outlineColor,
                        kerning,
                        direction
                    );
                }
                // Render the glyph with the current group styling.
                GlyphID glyph = CreateGlyphID(glyphs[glyphIndex].GetCodepoint(), currentGroup.style, currentGroup.hinting, 0);
                font.RenderGlyph(
                    renderer,
                    glyph,
                    position,
                    currentGroup.pointSize,
                    currentGroup.color,
                    kerning,
                    direction
                );
                position.x += direction == Typographic::TextDirection::LEFT_TO_RIGHT ? glyphs[glyphIndex].GetAdvance(currentGroup.pointSize) : -glyphs[glyphIndex].GetAdvance(currentGroup.pointSize);
                glyphIndex++;
                if (glyphIndex >= currentGroup.index)
                {
                    if ((currentGroup.style | mainStyle) & TTF_STYLE_UNDERLINE)
                    {
                        Vector2 underlinePos = Vector2(0, font.GetUnderlinePosition(currentGroup.pointSize));
                        Line underline(linePos + underlinePos, position + underlinePos);
                        underline.Draw(renderer, currentGroup.color);
                    }
                    if ((currentGroup.style | mainStyle) & TTF_STYLE_STRIKETHROUGH)
                    {
                        Vector2 strikethroughPos = Vector2(0, font.GetStrikethroughPosition(currentGroup.pointSize));
                        Line strikethrough(linePos + strikethroughPos, position + strikethroughPos);
                        strikethrough.Draw(renderer, currentGroup.color);
                    }
                    group++;
                    currentGroup = groups[group];
                }
            }
        }
    }

    void TextLayout::ComputeGlyphPosition(unsigned int glyphIndex, TextLine& line, const GlyphGroup& group)
    {
        GlyphMeta& meta = glyphs[glyphIndex];
        float advance = meta.GetAdvance(group.pointSize);
        if (lineWrap)
        {
            Vector2 dimensions = meta.GetDimensions(group.pointSize);
            if (line.size.x + dimensions.x > bbox.x && line.size.x > 0)
            {
                lines.push_back(line);
                line.size.x = 0;
                line.glyphIndex = glyphIndex;
            }
        }
        line.size.x += advance;
    }

    void TextLayout::ComputeLayout(TextLine& startLine, string lineBreakCharacters)
    {
        if (!(updateFlags & UPDATE_LAYOUT))
        {
            // Early out
            return;
        }

        // Clear all lines
        lines.clear();
        Vector2 startPosition = Vector2::Zero;
        for (unsigned int i = 0, counti = groups.size(); i < counti; i++)
        {
            for (unsigned int j = groups[i].index, countj = (i + 1 < counti ? groups[i + 1].index : glyphs.size()); j < countj; j++)
            {
                // First compute the positioning for each glyph on the line
                ComputeGlyphPosition(j, startLine, groups[i]);
            }
            // Now position the line itself
            ComputeLinePosition(startLine, startPosition);
        }
        // Append the final line
        lines.push_back(startLine);
        ComputeLinePosition(startLine, startPosition);

        updateFlags = 0;
    }

    void TextLayout::ComputeLayout(Renderer& renderer, Font& font, string& text, bool applyMarkup, string lineBreakCharacters)
    {
        if (!(updateFlags & UPDATE_ALL))
        {
            // Early out
            return;
        }

        bool isTag = false;
        bool isEscaped = false;
        string tagText;
        Uint32 boldTags = mainStyle & TTF_STYLE_BOLD ? 1 : 0;
        Uint32 italicTags = mainStyle & TTF_STYLE_ITALIC ? 1 : 0;
        Uint32 underlineTags = mainStyle & TTF_STYLE_UNDERLINE ? 1 : 0;
        Uint32 strikeTags = mainStyle & TTF_STYLE_STRIKETHROUGH ? 1 : 0;
        stack<SDL_Color> colours;
        colours.push(mainColor);

        size = Vector2::Zero;

        lines.clear();
        glyphs.clear();
        groups.clear();

        if (text.empty())
        {
            // Early out
            return;
        }

        GlyphGroup currentGroup = (GlyphGroup){0, pointSize, mainColor, (Uint8)mainStyle, 0, 0, Colors::BLACK};

        // Keeps track of the last ideal place in the line to make a line break
        Uint32 lineBreakIndex = 0;
        // Keeps track of the current line size.
        Vector2 lineDimensions = Vector2(0, font.GetLineDifference(pointSize));

        // The current text line being computed
        TextLine line = (TextLine){Vector2::Zero, lineDimensions, 0};

        // The advance width of the current word.
        float wordWidth = 0;
        // Counts consecutive space characters
        Uint32 whitespaceCount = 0;
        float whitespaceWidth = 0;

        font.BatchPackBegin(renderer);
        for (unsigned int i = 0, counti = text.length(); i < counti;)
        {
            // Extract UTF-8 character
            Uint8 bytes = (Uint8)max((int)Utilities::CheckUTF8(text[i]), 1);
            string utfChar = text.substr(i, bytes);
            i += bytes;

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
                        if (currentGroup.index < glyphs.size())
                        {
                            currentGroup.index = glyphs.size();
                            groups.push_back(currentGroup);
                        }
                        isTag = true;
                    }
                    else if (c == '>' && isTag)
                    {
                        isTag = false;
                        // Now parse the tag itself.
                        if (!tagText.empty())
                        {
                            Uint8 oldStyle = currentGroup.style;
                            if (!ParseTag(tagText, boldTags, italicTags, underlineTags, strikeTags, colours, currentGroup.style))
                            {
                                Logger::EngineLog().Warning("Failed to parse tag '<{0}>' in string '{1}'.", tagText, text);
                            }
                            if (currentGroup.color != colours.top() || currentGroup.style != oldStyle)
                            {
                                currentGroup.color = colours.top();
                                currentGroup.index = glyphs.size();
                                // TODO: outline, hinting, point size etc.
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

                glyphs.push_back(GlyphMeta(Utilities::GetCodepointUTF8(utfChar), font, currentGroup.style));

                // Compute the position of the glyph for the current line. If the current line is full, moves onto a newline.
                ComputeGlyphPosition(glyphs.size() - 1, line, currentGroup);

                // Only pack while this batch has space in the font atlas.
                if (font.GetBatchPackTotal() < font.GetAtlasMaxGlyphs())
                {
                    // Batch pack glyphs now to save processing time during rendering.
                    // TODO: outline, hinting
                    font.BatchPackGlyph(renderer, CreateGlyphID(glyphs.back().GetCodepoint(), currentGroup.style, 0, 0));
                }

                if (lineWrap && !wordBreak)
                {
                    if (utfChar[0] == ' ')
                    {
                        // Count whitespace
                        whitespaceCount++;
                        whitespaceWidth += glyphs.back().GetAdvance(pointSize);
                    }
                    else
                    {
                        wordWidth += glyphs.back().GetAdvance(pointSize) + whitespaceWidth;
                        whitespaceWidth = 0;
                        whitespaceCount = 0;
                    }
                    if (bytes <= 1)
                    {
                        for (auto c : lineBreakCharacters)
                        {
                            if (utfChar[0] == c)
                            {
                                // Set natural line break index
                                lineBreakIndex = glyphs.size();
                                lineDimensions.x += wordWidth;
                                wordWidth = 0;
                                break;
                            }
                        }
                    }
                }

            }
            else if (utfChar[0] == '\n')
            {
                // Add newline
                lineBreakIndex = glyphs.size();
                lines.push_back(line);
                line.glyphIndex = glyphs.size();
                line.size.x = 0;
            }

        }

        // Push back final line and update the groups
        lines.push_back(line);
        if (currentGroup.index < glyphs.size())
        {
            currentGroup.index = glyphs.size();
            groups.push_back(currentGroup);
        }

        // Finish render batching.
        font.BatchPackEnd(renderer);

        // Update line positioning
        ComputeLinePositions();

        updateFlags = 0;

    }

    void TextLayout::ComputeLinePosition(TextLine& line, Vector2& position)
    {
        line.position = Vector2::Zero;
        switch (alignment)
        {
        case Typographic::TextAlignment::RIGHT_ALIGNED:
            line.position.x = bbox.x - ceil(line.size.x);
            break;
        case Typographic::TextAlignment::CENTERED:
            line.position.x = (bbox.x / 2) - ceil(line.size.x / 2.0f);
            break;
        default:
            line.position.x = 0;
            break;
        }
        line.position.y = position.y;
        position.y += line.size.y;
    }

    void TextLayout::ComputeLinePositions()
    {
        if (!(updateFlags & UPDATE_LINES))
        {
            // Early out
            return;
        }
        Vector2 position = Vector2::Zero;
        for (TextLine& line : lines)
        {
            ComputeLinePosition(line, position);
        }
        updateFlags = 0;
    }

    void TextLayout::SetBounds(Vector2 bounds)
    {
        if (bbox != bounds)
        {
            bbox = bounds;
            updateFlags |= UPDATE_LAYOUT;
        }
    }

    Vector2 TextLayout::GetBounds()
    {
        return bbox;
    }

    void TextLayout::SetText(Renderer& renderer, Font& font, string text, bool applyMarkup)
    {
        updateFlags = UPDATE_ALL;
        ComputeLayout(renderer, font, text, applyMarkup);
    }

    void TextLayout::Update(Font& font)
    {
        if (updateFlags & UPDATE_LAYOUT)
        {
            TextLine startLine = (TextLine){Vector2::Zero, Vector2(0, font.GetLineDifference(pointSize)), 0};
            ComputeLayout(startLine);
        }
        else if (updateFlags & UPDATE_LINES)
        {
            ComputeLinePositions();
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
            updateFlags |= UPDATE_LINES;
        }
    }

    void TextLayout::SetDirection(Typographic::TextDirection textDirection)
    {
        if (direction != textDirection)
        {
            direction = textDirection;
            updateFlags |= UPDATE_LAYOUT;
        }
    }

    void TextLayout::SetKerning(bool kern)
    {
        if (kerning != kern)
        {
            kerning = kern;
            // TODO: kern table support
        }
    }

    void TextLayout::SetLineWrapping(bool wrap)
    {
        if (lineWrap != wrap)
        {
            lineWrap = wrap;
            updateFlags |= UPDATE_LAYOUT;
        }
    }

    void TextLayout::SetWordBreaking(bool midwordBreak)
    {
        if (wordBreak != midwordBreak)
        {
            wordBreak = midwordBreak;
            updateFlags |= UPDATE_LAYOUT;
        }
    }

    void TextLayout::SetIgnoringWhitespace(bool ignoreSpaces)
    {
        if (ignoreWhitespace != ignoreSpaces)
        {
            ignoreWhitespace = ignoreSpaces;
            updateFlags |= UPDATE_LAYOUT;
        }
    }

    void TextLayout::SetPointSize(float ptSize)
    {
        if (pointSize != ptSize)
        {
            pointSize = ptSize;
            // TODO: only update layout, but make sure first group pointSize is set.
            updateFlags = UPDATE_ALL;
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
                if (boldTags == 0)
                {
                    style = style & ~TTF_STYLE_BOLD;
                }
            }
            else if (tagText[1] == 'i')
            {
                italicTags = max((Uint32)0, italicTags - 1);
                if (italicTags == 0)
                {
                    style = style & ~TTF_STYLE_ITALIC;
                }
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
                style = style | TTF_STYLE_BOLD;
            }
            else if (tagText[0] == 'i')
            {
                italicTags++;
                style = style | TTF_STYLE_ITALIC;
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
