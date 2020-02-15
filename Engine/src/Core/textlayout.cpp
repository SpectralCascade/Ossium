#include <stack>

#include "utf8.h"
#include "textlayout.h"

using namespace std;

namespace Ossium
{

    //
    // GlyphBatch
    //

    GlyphBatch::GlyphBatch(Vector2 startPos, SDL_Color mainColor, int addStyle)
    {
        position = startPos;
        color = mainColor;
        additiveStyle = addStyle;
    }

    //
    // TextLayout
    //

    void TextLayout::Render(Renderer& renderer, string text, Vector2 position, Font& font, float pointSize, Rect boundingBox, SDL_Color mainColor, int mainStyle, bool applyMarkup, string lineBreakCharacters)
    {
        // Markup helper variables for parsing tags
        // TODO: make tags more extensible?
        bool isTag = false;
        bool isEscaped = false;
        string tagText;
        Uint32 boldTags = 0;
        Uint32 italicTags = 0;
        Uint32 underlineTags = 0;
        Uint32 strikeTags = 0;
        stack<SDL_Color> colours;
        colours.push(mainColor);
        stack<int> styles;
        styles.push(mainStyle);

        // Keep track of glyphs that have been batch packed.
        vector<GlyphBatch> batched;
        GlyphBatch currentBatch = GlyphBatch(position, mainColor, mainStyle);

        // For line break tracking
        string currentWord;
        Vector2 safeBreak = position;

        // Batch pack each glyph
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
                        isTag = true;
                    }
                    else if (c == '>' && isTag)
                    {
                        isTag = false;
                        // Now parse the tag itself.
                        if (!tagText.empty())
                        {
                            if (!ParseTag(tagText, boldTags, italicTags, underlineTags, strikeTags, colours, styles))
                            {
                                Logger::EngineLog().Warning("Failed to parse tag '<{0}>' in string '{1}'.", tagText, text);
                            }
                            bool updateBatched = false;
                            if (mainColor != colours.top())
                            {
                                mainColor = colours.top();
                                updateBatched = true;
                            }
                            if (mainStyle != styles.top())
                            {
                                mainStyle = styles.top();
                                updateBatched = true;
                            }
                            if (updateBatched)
                            {
                                batched.push_back(currentBatch);
                                currentBatch.glyphs.clear();
                                currentBatch.additiveStyle = mainStyle;
                                currentBatch.color = mainColor;
                                currentBatch.position = position;
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

            if (!isTag && !wasTag)
            {
                // Pack the glyph. If the batch reaches the maximum number of atlas glyphs, render all the glyphs to the atlas, then render the text string so far.
                //Logger::EngineLog().Info("Packing glyph for {0}...", utfChar);
                Glyph* toBatch = font.GetGlyph(renderer, utfChar, style);

                // Increase approximate layout position (not taking kerning into account).
                position += toBatch->GetChange(font.GetLoadedPointSize(), pointSize, direction);

                // Check if we are in bounds. If not, start a new glyph batch for the next line.
                switch (direction)
                {
                case Typographic::TextDirection::LEFT_TO_RIGHT:
                    if (position.x >= boundingBox.x + boundingBox.w)
                    {
                        position.x = boundingBox.x;
                        position.y += font.GetLineDifference(pointSize);
                    }
                    break;
                case Typographic::TextDirection::RIGHT_TO_LEFT:
                    if (position.x <= boundingBox.x)
                    {
                        position.x = boundingBox.x + boundingBox.w;
                        position.y += font.GetLineDifference(pointSize);
                    }
                    break;
                case Typographic::TextDirection::TOP_TO_BOTTOM:
                    if (position.y >= boundingBox.y + boundingBox.h)
                    {
                        position.y = boundingBox.y;
                        position.x += font.GetLineDifference(pointSize);
                    }
                    break;
                }

                if (lineWrap)
                {
                    if (bytes <= 1)
                    {
                        // Line break after certain delimiters
                        for (auto c : lineBreakCharacters)
                        {
                            if (c == utfChar[0])
                            {
                                safeBreak = position;
                                currentWord.clear();
                                break;
                            }
                        }
                    }
                    else
                    {
                        currentWord += utfChar;
                    }
                }

                currentBatch.glyphs.push_back(toBatch);
                if (font.BatchPackGlyph(renderer, toBatch) >= font.GetAtlasMaxGlyphs() - 1)
                {
                    font.BatchPackEnd(renderer);

                    batched.push_back(currentBatch);
                    currentBatch.glyphs.clear();

                    // At this point, render all batched glyphs before continuing with a new batch
                    for (auto batch : batched)
                    {
                        RenderBatch(renderer, batch, pointSize, font);
                    }
                    batched.clear();

                    // Then continue batching
                    font.BatchPackBegin(renderer);
                }
            }
        }
        font.BatchPackEnd(renderer);
        // Render any remaining batches
        for (auto batch : batched)
        {
            RenderBatch(renderer, batch, pointSize, font);
        }
        batched.clear();
    }

    bool TextLayout::ParseTag(string tagText, int& boldTags, int& italicTags, int& underlineTags, int& strikeTags, stack<SDL_Color>& colors, int& style)
    {
        unsigned int tagTextLength = tagText.length();
        bool success = true;
        if (tagText[0] == '/' && tagTextLength > 1)
        {
            if (tagText[0] == 'b')
            {
                boldTags = max(0, boldTags - 1);
            }
            else if (tagText[0] == 'i')
            {
                italicTags = max(0, italicTags - 1);
            }
            else if (tagText[0] == 'u')
            {
                underlineTags = max(0, underlineTags - 1);
                if (underlineTags == 0)
                {
                    style = style & TTF_STYLE_STRIKETHROUGH;
                }
            }
            else if (tagText[0] == 's')
            {
                strikeTags = max(0, strikeTags - 1);
                if (strikeTags == 0)
                {
                    style = style & TTF_STYLE_UNDERLINE;
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
                style |= TTF_STYLE_UNDERLINE;
            }
            else if (tagText[0] == 's')
            {
                strikeTags++;
                style |= TTF_STYLE_STRIKETHROUGH;
            }
            else if (tagTextLength >= 13 && tagText.substr(0, 7) == "color=#")
            {
                // Get the actual colour
                tagText = Utilities::SplitRight(tagText, '#', "FF0000");
                tagTextLength = tagText.length();
                Uint8 alpha = 0xFF;
                SDL_Color mainColor = colors.top();

                Uint32 converted = Utilities::ToUint32FromHex(tagText);
                switch (tagTextLength)
                {
                case 8:
                    alpha = converted & 0x000000FF;
                    converted >> 8;
                case 6:
                    mainColor = Color(converted & 0x00FF0000, converted & 0x0000FF00, converted & 0x000000FF, alpha);
                    colors.push(mainColor);
                    break;
                default:
                    Logger::EngineLog().Warning("Invalid color tag in string '{0}'!", text);
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
/*
    unsigned int TextLayout::DrawLine(Renderer& renderer, const string& text, Vector2& position, Font& font, unsigned int start, unsigned int end, Uint32& lastID, double angle = 0.0, float deltaTime = 0.0, float waveAmount = 0.0f, float waveSpeed = 10.0f, float phaseIncrement = 45.0f, float psize = 24.0f, int style = TTF_STYLE_NORMAL, float mipBias = 0.5f)
    {
        static float a;
        a += deltaTime * waveSpeed;
        float yoffset = sin(a) * waveAmount;
        float inc = 0.0f;

        while (start < end)
        {
            inc += phaseIncrement;
            yoffset = sin(a + inc) * waveAmount;

            // Extract UTF-8 character
            Uint8 bytes = (Uint8)max((int)Utilities::CheckUTF8(text[start]), 1);
            string utfChar = text.substr(start, bytes);
            start += bytes;

            Glyph* glyph = font.GetGlyph(renderer, utfChar, style);
            if (glyph != nullptr)
            {
                if (lastID != 0 && direction != Typographic::TextDirection::TOP_TO_BOTTOM)
                {
                    // Apply kerning manually
                    position.x += TTF_GetFontKerningSizeGlyphs(font.GetFont(), (Uint16)lastID, (Uint16)glyph->GetCodePointUTF8());
                }
                lastID = glyph->GetID();
            }
            else
            {
                lastID = 0;
            }
            position.y += yoffset;
            position = font.RenderGlyph(renderer, glyph, position, psize, Colors::GREEN, true, false, SDL_BLENDMODE_BLEND, mipBias, angle);
            position.y -= yoffset;

            //Logger::EngineLog().Info("Rendering character {0} [index {1} until {2}]", (char)glyph->GetCodePointUTF8(), start, end);
            //style = (style + 1) % 3;
        }
        return start;
    }
*/
    Vector2 TextLayout::RenderBatch(Renderer& renderer, GlyphBatch& batch, float pointSize, Font& font)
    {
        for (auto glyph : batch.glyphs)
        {
            batch.position = font.RenderGlyph(
                renderer,
                glyph,
                direction == Typographic::TextDirection::RIGHT_TO_LEFT && glyph != nullptr ?
                    batch.position - ((float)glyph->GetClip().w * (pointSize / (float)font.GetLoadedPointSize())) : batch.position,
                pointSize,
                batch.color,
                kerning,
                direction
            );
        }
        if (batch.additiveStyle & TTF_STYLE_UNDERLINE)
        {
            Vector2 underlinePos = Vector2(0, font.GetUnderlinePosition(pointSize));
            Line underline(startPos + underlinePos, position + underlinePos);
            underline.Draw(renderer, batch.color);
        }
        if (batch.additiveStyle & TTF_STYLE_STRIKETHROUGH)
        {
            Vector2 strikethroughPos = Vector2(0, font.GetStrikethroughPosition(pointSize));
            Line strikethrough(startPos + strikethroughPos, position + strikethroughPos);
            strikethrough.Draw(renderer, batch.color);
        }
        return batch.position;
    }

}
