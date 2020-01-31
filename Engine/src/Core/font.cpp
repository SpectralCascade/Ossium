/** COPYRIGHT NOTICE
 *
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
#include "font.h"
#include "logging.h"
#include "utf8.h"

using namespace std;

namespace Ossium
{

    //
    // TextStyle
    //

    TextStyle::TextStyle(string font, int fontSize, SDL_Color color, int hint, int kern, int outlineThickness,
        int styling, int renderingMode, SDL_Color backgroundColor)
    {
        fontPath = font;
        ptsize = fontSize;
        fg = color;
        hinting = hint;
        kerning = kern;
        outline = outlineThickness;
        style = styling;
        rendermode = renderingMode;
        bg = backgroundColor;
    }

    //
    // Glyph
    //

    Glyph::Glyph(Uint32 codepoint, Uint32 style, Uint32 outline)
    {
        type = CreateGlyphID(codepoint, style, outline);
    }

    SDL_Rect Glyph::GetClip()
    {
        return clip;
    }

    void Glyph::UpdateMeta(Uint32 index, SDL_Rect quad)
    {
        clip = quad;
        atlasIndex = index;
    }

    Uint32 Glyph::GetAtlasIndex()
    {
        return atlasIndex;
    }

    SDL_Rect Glyph::GetBoundingBox()
    {
        return bbox;
    }

    int Glyph::GetAdvance()
    {
        return advanceMetric;
    }

    Uint32 Glyph::GetCodePointUTF8()
    {
        return type & GLYPH_UTF8_MASK;
    }

    GlyphID Glyph::GetID()
    {
        return type;
    }

    //
    // Font
    //

    REGISTER_RESOURCE(Font);

    Font::Font()
    {
        font = NULL;
    }

    Font::~Font()
    {
        Free();
    }

    void Font::Free()
    {
        FreeGlyphs();
        FreeAtlas();
        if (font != NULL)
        {
            TTF_CloseFont(font);
            font = NULL;
        }
    }

    void Font::FreeAtlas()
    {
        atlas.PopGPU();
    }

    bool Font::Load(string guid_path, int maxPointSize)
    {
        Free();
        path = guid_path;
        if (maxPointSize <= 0)
        {
            /// Load up a default font size
            loadedPointSize = 24;
        }
        else
        {
            loadedPointSize = maxPointSize;
        }
        font = TTF_OpenFont(guid_path.c_str(), loadedPointSize);
        if (font == NULL)
        {
            Logger::EngineLog().Error("Failed to open font '{0}'! TTF_Error: {1}", guid_path, TTF_GetError());
        }
        return font != NULL;
    }

    bool Font::LoadAndInit(string guid_path, Renderer& renderer, int maxPointSize, Uint16 targetTextureSize, int atlasPadding, Uint32 pixelFormat, Uint32 glyphCacheLimit)
    {
        return Load(guid_path, maxPointSize) && Init(guid_path, renderer, targetTextureSize, atlasPadding, pixelFormat, glyphCacheLimit);
    }

    bool Font::Init(string guid_path, Renderer& renderer, Uint16 targetTextureSize, int atlasPadding, Uint32 pixelFormat, Uint32 glyphCacheLimit)
    {
        padding = atlasPadding;

        fontHeight = TTF_FontHeight(font);
        if (fontHeight <= 0)
        {
            Logger::EngineLog().Error("Font has invalid height!");
            return false;
        }
        // Compute actual texture size using font height and target texture size
        Uint32 actualTextureSize = (targetTextureSize / fontHeight) * GetAtlasCellSize();

        // Create the atlas surface and push onto the GPU.
        atlas.CreateEmptySurface(actualTextureSize, actualTextureSize, pixelFormat);
        cacheLimit = glyphCacheLimit;
        if (atlas.GetSurface() != NULL)
        {
            return atlas.PushGPU(renderer, pixelFormat, SDL_TEXTUREACCESS_TARGET) != NULL;
        }
        return false;
    }

    SDL_Surface* Font::GenerateFromText(Renderer& renderer, string text, SDL_Color color, int hinting, int kerning, int style, int renderMode, SDL_Color bgColor, int outline, Uint32 wrapLength)
    {
        TTF_Font* font = GetFont();
        if (font == NULL)
        {
            Logger::EngineLog().Error("Failed to get font for on-the-fly text generation!");
            return NULL;
        }

        // Cache original settings so they can be restored once done.
        int oldHinting = TTF_GetFontHinting(font);
        int oldKerning = TTF_GetFontKerning(font);
        int oldOutline = TTF_GetFontOutline(font);
        int oldStyle = TTF_GetFontStyle(font);

        // Configure font
        TTF_SetFontKerning(font, (int)kerning);
        if (oldHinting != hinting)
        {
            TTF_SetFontHinting(font, hinting);
        }
        if (outline != oldOutline)
        {
            TTF_SetFontOutline(font, outline);
        }
        if (style != oldStyle)
        {
            TTF_SetFontStyle(font, style);
        }

        SDL_Surface* tempSurface = NULL;

        switch (renderMode)
        {
            case RENDERTEXT_SHADED:
            {
                tempSurface = TTF_RenderUTF8_Shaded(font, text.c_str(), color, bgColor);
                break;
            }
            case RENDERTEXT_BLEND:
            {
                tempSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
                break;
            }
            case RENDERTEXT_BLEND_WRAPPED:
            {
                tempSurface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, wrapLength);
                break;
            }
            default:
            {
                tempSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
                break;
            }
        }

        // Restore settings
        TTF_SetFontKerning(font, oldKerning);
        if (oldHinting != hinting)
        {
            TTF_SetFontHinting(font, oldHinting);
        }
        if (outline != oldOutline)
        {
            TTF_SetFontOutline(font, oldOutline);
        }
        if (style != oldStyle)
        {
            TTF_SetFontStyle(font, oldStyle);
        }

        return tempSurface;
    }

    Glyph* Font::GetGlyph(Renderer& renderer, string utfChar, int style, int outline)
    {
        GlyphID id = CreateGlyphID(Utilities::GetCodepointUTF8(utfChar), style, outline);

        // Limit style to main styles; strike through and underline styles don't need to be cached as they can be drawn dynamically.
        style = style & (TTF_STYLE_BOLD | TTF_STYLE_ITALIC);

        Glyph* glyph = nullptr;

        auto itr = glyphs.find(id);
        if (itr != glyphs.end())
        {
            // Already cached, return the glyph
            glyphCache.Access(id);
            glyph = itr->second;
        }
        else
        {
            if (!(atlas.GetTextureAccessMode() & (SDL_TEXTUREACCESS_TARGET | SDL_TEXTUREACCESS_STREAMING)))
            {
                Logger::EngineLog().Error("Cannot generate font glyph as the font has not been initialised!");
            }
            // Attempt to add the glyph to the atlas
            // TODO: upgrade encoding from UCS-2 to UCS-4 (or even better, UTF-8) on next SDL_TTF update.
            // Currently the glyphs are limited due to lack of proper character encoding support in SDL_TTF.
            Uint16 ucs2 = Utilities::ConvertUTF8ToUCS2(utfChar);
            if (ucs2)
            {
                // Check if the glyph exists
                if (TTF_GlyphIsProvided(font, ucs2))
                {
                    SDL_Surface* renderedGlyph = GenerateFromText(renderer, utfChar, Colors::WHITE, 0, 0, style, RENDERTEXT_BLEND);
                    if (renderedGlyph != NULL)
                    {

                        if (glyphCache.Size() >= cacheLimit)
                        {
                            // Replace a glyph in the glyph cache
                            Uint32 toReplace = glyphCache.GetLRU();
                            // Remove from the cache
                            glyphCache.PopLRU();
                            auto replaceItr = glyphs.find(toReplace);
                            if (replaceItr != glyphs.end())
                            {
                                // Remove the current map entry
                                glyph = replaceItr->second;
                                glyphs.erase(replaceItr);
                            }
                            else
                            {
                                // This should never happen. If it does there's a problem in code.
                                Logger::EngineLog().Error("Font system failure! LRU decimal code point {0} not found in glyphs map :(", toReplace);
                            }
                        }
                        else
                        {
                            // Create a new glyph
                            glyph = new Glyph(id & GLYPH_UTF8_MASK, (id & GLYPH_STYLE_MASK) >> GLYPH_STYLE_SHIFT, (id & GLYPH_OUTLINE_MASK) >> GLYPH_OUTLINE_SHIFT);
                            //Logger::EngineLog().Verbose("Created new glyph from code point {0}.", codepoint);
                        }

                        // Glyph manages surface memory now
                        glyph->cached.SetSurface(renderedGlyph);
                        glyph->type = id;

                        // Set glyph metrics
                        TTF_GlyphMetrics(font, ucs2, &glyph->bbox.x, &glyph->bbox.y, &glyph->bbox.w, &glyph->bbox.h, &glyph->advanceMetric);
                        glyph->bbox.w -= glyph->bbox.x;
                        glyph->bbox.h -= glyph->bbox.y;

                    }
                    else
                    {
                        Logger::EngineLog().Error("Failed to render glyph to surface. TTF_Error: {0}", TTF_GetError());
                    }
                }
                else
                {
                    Logger::EngineLog().Verbose("Glyph is not provided for UTF-8 character {0}.", utfChar);
                }
            }
            else if (!utfChar.empty() && utfChar[0] != 0)
            {
                // Unsupported character
                Logger::EngineLog().Verbose("Failed to get glyph for UTF-8 character {0} as it cannot be converted to a valid UCS-2 code point (SDL_TTF 2.0.15 limitation).", utfChar);
            }

        }

        // Add the glyph to the map and update the LRU cache
        glyphs[id] = glyph;
        glyphCache.Access(id);

        return glyph;
    }

    void Font::BatchPackBegin(Renderer& renderer)
    {
        batched = 0;

        if (IsBatchPacking())
        {
            // Batching is still in progress, so no need to configure the renderer
            return;
        }

        // Configure renderer
        SDL_Renderer* render = renderer.GetRendererSDL();

        SDL_Texture* target = atlas.GetTexture();
        if (target == NULL)
        {
            Logger::EngineLog().Warning("Font atlas is uninitialised, cannot pack glyphs!");
            return;
        }

        // Set render target to atlas
        originalTarget = SDL_GetRenderTarget(render);
        SDL_SetRenderTarget(render, target);

        // Configure blending so old pixels are overwritten when glyphs are packed.
        SDL_GetRenderDrawBlendMode(render, &originalBlending);
        SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_NONE);
        if (originalBlending == SDL_BLENDMODE_INVALID)
        {
            // Just in case something went wrong getting the blend mode
            originalBlending = SDL_BLENDMODE_NONE;
        }

    }

    Uint32 Font::BatchPackGlyph(Renderer& renderer, Glyph* glyph)
    {
        if (glyph == nullptr)
        {
            // Early out
            return batched;
        }

        // First check if the glyph is already packed
        Uint32 index = glyph->GetAtlasIndex();
        if (index != 0)
        {
            // Already packed, just update the cache and early out.
            textureCache.Access(index);
            return batched;
        }

        // Find a spot for the glyph to be rendered
        if (textureCache.Size() == GetAtlasMaxGlyphs())
        {
            // Overwrite LRU index, replace the glyph that was there
            index = textureCache.GetLRU();
            auto mapped = atlasGlyphMap.find(index);
            bool failedReplace = true;
            if (mapped != atlasGlyphMap.end())
            {
                auto glyphItr = glyphs.find(mapped->second);
                if (glyphItr != glyphs.end())
                {
                    if (glyphItr->second != nullptr)
                    {
                        // Replaced glyph is unpacked
                        //Logger::EngineLog().Verbose("LRU character = {0}, replacing with {1}", (char)glyphItr->second->GetCodePointUTF8(), (char)glyph->GetCodePointUTF8());
                        glyphItr->second->atlasIndex = 0;
                    }
                    failedReplace = false;
                }
            }
            if (failedReplace)
            {
                Logger::EngineLog().Error("Failed to find mapped glyph for atlas index {0}! Either the cache is smaller than GetAtlasMaxGlyphs() or this font is not initialised.", index);
            }
        }
        else
        {
            // Use the next available index (must be non-zero, hence +1).
            index = textureCache.Size() + 1;
        }

        // Update atlas cache
        textureCache.Access(index);
        atlasGlyphMap[index] = glyph->GetID();

        SDL_Rect dest = GetAtlasCell(index);
        SDL_Rect cell = dest;
        //Logger::EngineLog().Verbose("Packing glyph at index {0} ({1}), max glyphs = {2}, texture cache size = {3}", index, dest, GetAtlasMaxGlyphs(), textureCache.Size());

        if (dest.w != 0 && dest.h != 0)
        {
            SDL_Renderer* render = renderer.GetRendererSDL();

            // Render the glyph to the atlas.
            if (glyph->cached.GetWidth() > dest.w || glyph->cached.GetHeight() > dest.h)
            {
                Logger::EngineLog().Warning("Attempting to pack glyph that is bigger than the atlas cell size! It'll be a bit squashed.");
            }
            // TODO: IMPORTANT: rather than squishing unevenly, scale down and store the scale difference for later.
            dest.w = min(dest.w, glyph->cached.GetWidth());
            dest.h = min(dest.h, glyph->cached.GetHeight());

            //Logger::EngineLog().Verbose("Rendering glyph {0} to font atlas...", glyph->GetCodePointUTF8());

            // Overwrite whatever was in the atlas cell before
            SDL_SetRenderDrawColor(render, 0, 0, 0, 0xFF);
            SDL_RenderFillRect(render, &cell);

            // Render the actual glyph
            glyph->cached.PushGPU(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC);
            SDL_Rect clip = {0, 0, glyph->cached.GetWidth(), glyph->cached.GetHeight()};
            SDL_RenderCopy(render, glyph->cached.GetTexture(), &clip, &dest);
            glyph->cached.PopGPU();

            // Update glyph metrics
            glyph->UpdateMeta(index, dest);

            batched++;
        }

        return batched;
    }

    void Font::BatchPackEnd(Renderer& renderer)
    {
        if (IsBatchPacking())
        {
            SDL_Renderer* render = renderer.GetRendererSDL();

            if (batched != 0)
            {
                // Render everything
                SDL_RenderPresent(render);
                batched = 0;
            }

            // Reconfigure renderer back to how it was originally
            SDL_SetRenderTarget(render, originalTarget);
            SDL_SetRenderDrawBlendMode(render, originalBlending);

            // When originalBlending == SDL_BLENDMODE_INVALID, batching is not in progress.
            originalBlending = SDL_BLENDMODE_INVALID;
            originalTarget = NULL;
        }
    }

    Uint32 Font::GetBatchPackTotal()
    {
        return batched;
    }

    bool Font::IsBatchPacking()
    {
        return originalBlending != SDL_BLENDMODE_INVALID;
    }

    void Font::Render(Renderer& renderer, SDL_Rect dest, SDL_Rect clip, SDL_Color color, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        atlas.Render(renderer.GetRendererSDL(), dest, &clip, origin, angle, color, blending, flip);
    }

    Vector2 Font::RenderGlyph(Renderer& renderer, Glyph* glyph, Vector2 position, float pointSize, SDL_Color color, bool kerning, bool rtl, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        SDL_Rect dest = {(int)(position.x), (int)(position.y), 0, 0};
        if (glyph == nullptr)
        {
            // Invalid glyph, render a box instead
            int boxPadding = fontHeight / 10;
            dest.x += boxPadding;
            dest.w = (fontHeight / 2);
            dest.h = (fontHeight / 3) * 2;
            dest.y += fontHeight / 8;
            SDL_Color oldColor = renderer.GetDrawColor();
            renderer.SetDrawColor(color);
            SDL_RenderDrawRect(renderer.GetRendererSDL(), &dest);
            renderer.SetDrawColor(oldColor);
            return position + Vector2(rtl ? -(boxPadding * 2 + dest.w) : (boxPadding * 2 + dest.w), 0);
        }
        // TODO: proper sizing with point size
        Vector2 size = Vector2(glyph->cached.GetWidth(), glyph->cached.GetHeight());
        // TODO: position based on glyph metrics such as baseline position etc. rather than using the centre of the glyph
        dest = {dest.x, dest.y, (int)(size.x), (int)(size.y)};
        Render(renderer, dest, glyph->GetClip(), color, blending, angle, origin, flip);
        // TODO: kerning
        size.x = glyph->GetAdvance();
        return position + Vector2(rtl ? -size.x : size.x, 0);
    }

    void Font::FreeGlyphs()
    {
        for (auto itr : glyphs)
        {
            if (itr.second != nullptr)
            {
                delete itr.second;
                itr.second = nullptr;
            }
        }
        glyphs.clear();
        glyphCache.Clear();
        textureCache.Clear();
    }

    TTF_Font* Font::GetFont()
    {
        return font;
    }

    Uint32 Font::GetAtlasSize()
    {
        return atlas.GetHeight();
    }

    Uint32 Font::GetAtlasMaxGlyphs()
    {
        int total = GetAtlasSize() / GetAtlasCellSize();
        return total * total;
    }

    int Font::GetAtlasCellSize()
    {
        return fontHeight + (padding * 2);
    }

    SDL_Rect Font::GetAtlasCell(Uint32 index)
    {
        SDL_Rect rect = {0, 0, 0, 0};
        if (index < 1 || index > GetAtlasMaxGlyphs() || atlas.GetTexture() == NULL)
        {
            Logger::EngineLog().Warning("Failed to get atlas cell for index {0} (note: index must be between 1 and {1} inclusive and atlas must be initialised)", index, GetAtlasMaxGlyphs());
            return rect;
        }
        int linearPosition = ((index - 1) * GetAtlasCellSize());
        rect.x = linearPosition % atlas.GetWidth();
        rect.y = ((linearPosition - rect.x) / atlas.GetWidth()) * GetAtlasCellSize();
        rect.w = GetAtlasCellSize();
        rect.h = rect.w;
        if (padding > 0)
        {
            // Get the true destination rect with padding
            rect.x += padding;
            rect.y += padding;
            rect.w -= padding * 2;
            rect.h -= padding * 2;
        }
        return rect;
    }

}
