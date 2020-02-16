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

    void TextLine::AddGlyph(Glyph* glyph)
    {
        glyphs.push_back(glyph);
        width += glyph != nullptr ? (float)glyph->GetAdvance() : invalidDimensions.x;
    }

    void TextLine::BeginSegment(Glyph* glyph, Uint8 style, SDL_Color color)
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
        else if (glyphs.back() == nullptr)
        {
            return GetWidth();
        }
        return ((width - (float)glyphs.back()->GetAdvance()) + ((float)glyphs.back()->GetBoundingBox().w)) * glyphScale;
    }

    TextLineSegment TextLine::GetCurrentSegment()
    {
        return segments.back();
    }

    const vector<Glyph*>& TextLine::GetGlyphs()
    {
        return glyphs;
    }

    const vector<TextLineSegment>& TextLine::GetSegments()
    {
        return segments;
    }

    //
    // TextLayout
    //

    void TextLayout::Render(Renderer& renderer, string text, Font& font, float pointSize, Rect boundingBox, SDL_Color mainColor, int mainStyle, bool applyMarkup, string lineBreakCharacters)
    {
        // Markup helper variables for keeping track of tags
        // TODO: make tags more extensible?
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

        // Each line of text
        vector<TextLine> lines;
        lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));

        Vector2 linePosition = Vector2(boundingBox.x, boundingBox.y);

        // Batch pack each glyph
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
                Glyph* toBatch = font.GetGlyph(renderer, utfChar, (boldTags > 0 ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL) | (italicTags > 0 ? TTF_STYLE_ITALIC : TTF_STYLE_NORMAL));

                bool lineChange = false;

                // If there is more than one glyph on the line, and the line exceeds the bounding box, OR there is a newline character, wrap to a new line.
                if (lineWrap && ((lines.back().GetGlyphs().size() > 0) && (!(ignoreWhitespace && utfChar[0] == ' ')) &&
                    (lines.back().GetWidth() + (toBatch != nullptr ? toBatch->GetDimensions(font.GetLoadedPointSize(), pointSize).x : font.GetFontHeight(pointSize) * 0.5f) >= boundingBox.w))
                )
                {
                    lineChange = true;
                    lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));
                }

                // Add glyph to the current line
                if (addLineSegment && !lineChange)
                {
                    lines.back().BeginSegment(toBatch, style, mainColor);
                }
                else
                {
                    lines.back().AddGlyph(toBatch);
                }

                // If the batch reaches the maximum number of atlas glyphs, render all the glyphs to the atlas, then render the text lines so far.
                if (font.BatchPackGlyph(renderer, toBatch) >= font.GetAtlasMaxGlyphs() - 1)
                {
                    font.BatchPackEnd(renderer);

                    // Render all lines except for the last line.
                    // TODO: account for lines which use up more glyphs than allowed in font atlas!
                    for (Uint32 i = 0, counti = lines.size() - 1; i < counti; i++)
                    {
                        RenderLine(renderer, lines[i], linePosition, pointSize, font);
                        linePosition.y += font.GetLineDifference(pointSize);
                    }
                    // Remove the lines that have been rendered, but not the last line.
                    TextLine current = lines.back();
                    lines.clear();
                    lines.push_back(current);

                    // Then continue batching
                    font.BatchPackBegin(renderer);
                }
            }
            else if (utfChar[0] == '\n')
            {
                // Add newline
                lines.push_back(TextLine(font.GetLoadedPointSize(), pointSize, mainColor, style, font.GetInvalidGlyphDimensions(pointSize)));
            }
        }
        font.BatchPackEnd(renderer);

        // Render all remaining lines
        for (TextLine line : lines)
        {
            RenderLine(renderer, line, linePosition, pointSize, font);
            linePosition.y += font.GetLineDifference(pointSize);
        }

    }

    void TextLayout::RenderLine(Renderer& renderer, TextLine& line, Vector2 position, float pointSize, Font& font)
    {
        if (line.GetGlyphs().empty())
        {
            // Early out
            return;
        }

        Vector2 startPos = position;
        for (Uint32 i = 0, counti = line.GetSegments().size(); i < counti; i++)
        {
            Uint32 nextSegment = i + 1 < line.GetSegments().size() ? line.GetSegments()[i + 1].index : line.GetGlyphs().size();
            for (Uint32 index = line.GetSegments()[i].index; index < nextSegment; index++)
            {
                position = font.RenderGlyph(
                    renderer,
                    line.GetGlyphs()[index],
                    position,
                    pointSize,
                    line.GetSegments()[i].color,
                    kerning,
                    direction
                );
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
