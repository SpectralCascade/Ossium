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

    bool Font::LoadAndInit(string guid_path, int maxPointSize, Renderer& renderer, Uint16 targetTextureSize, int atlasPadding, Uint32 pixelFormat, Uint32 glyphCacheLimit, int maxMipmaps)
    {
        return Load(guid_path, maxPointSize) && Init(guid_path, renderer, targetTextureSize, atlasPadding, pixelFormat, glyphCacheLimit, maxMipmaps);
    }

    bool Font::Init(string guid_path, Renderer& renderer, Uint16 targetTextureSize, int atlasPadding, Uint32 pixelFormat, Uint32 glyphCacheLimit, int mipDepth)
    {
        padding = atlasPadding;
        mipOffsets.clear();

        fontHeight = TTF_FontHeight(font);
        if (fontHeight <= 0)
        {
            Logger::EngineLog().Error("Font has invalid height! Cannot initialise.");
            return false;
        }

        // Compute cell size
        cellSize = {fontHeight + (padding * 2) + (int)ceil((float)fontHeight * 1.5f), fontHeight + (padding * 2)};

        fontAscent = TTF_FontAscent(font);
        fontDescent = TTF_FontDescent(font);

        if (mipDepth <= 0)
        {
            // Automatically calculate mipmap depth for minimum 8 points font size
            mipmapDepth = (int)(sqrt((float)loadedPointSize / 8.0f));
        }
        else
        {
            mipmapDepth = mipDepth;
        }

        // Compute mipmap offsets
        SDL_Rect offsetRect = {0, 0, fontHeight, fontHeight};
        mipOffsets.push_back(offsetRect);
        offsetRect.x = fontHeight;
        offsetRect.w = (int)round((float)fontHeight / 2.0f);
        offsetRect.h = offsetRect.w;
        for (int i = 0; i < mipmapDepth; i++)
        {
            mipOffsets.push_back(offsetRect);
            offsetRect.y += offsetRect.h;
            offsetRect.w = (int)round((float)offsetRect.w / 2.0f);
            offsetRect.h = offsetRect.w;
        }

        // Open up the font at different mipmap levels
        for (auto f : mipmapFonts)
        {
            if (f != NULL)
            {
                TTF_CloseFont(f);
            }
            f = NULL;
        }
        mipmapFonts.clear();
        float pointSize = (float)loadedPointSize;
        for (int level = 0; level < mipmapDepth; level++)
        {
            pointSize = pointSize * 0.5f;
            Logger::EngineLog().Info("Loading font '{0}' at point size {1}", guid_path, pointSize);
            mipmapFonts.push_back(TTF_OpenFont(guid_path.c_str(), (int)pointSize));
            if (mipmapFonts.back() == NULL)
            {
                Logger::EngineLog().Error("TTF_Error opening font: {0}", TTF_GetError());
            }
        }

        // Compute actual texture size using font height and target texture size
        actualTextureSize = {(targetTextureSize / cellSize.x) * cellSize.x, (targetTextureSize / cellSize.y) * cellSize.y};
        maxAtlasGlyphs = (actualTextureSize.x / cellSize.x) * (actualTextureSize.y / cellSize.y);

        cacheLimit = glyphCacheLimit;

        // Create the atlas surface and push onto the GPU.
        atlas.CreateEmptySurface(actualTextureSize.x, actualTextureSize.y, pixelFormat);
        if (atlas.GetSurface() != NULL)
        {
            return atlas.PushGPU(renderer, SDL_TEXTUREACCESS_TARGET) != NULL;
        }
        return false;
    }

    SDL_Surface* Font::GenerateFromText(Renderer& renderer, string text, SDL_Color color, int hinting, int kerning, int style, int renderMode, SDL_Color bgColor, int outline, Uint32 wrapLength, TTF_Font* f)
    {
        if (f == NULL)
        {
            f = font;
            if (f == NULL)
            {
                Logger::EngineLog().Error("TTF Font is not loaded for on-the-fly text generation!");
                return NULL;
            }
        }

        int oldHinting = TTF_GetFontHinting(f);
        int oldOutline = TTF_GetFontOutline(f);
        int oldStyle = TTF_GetFontStyle(f);

        // Configure font
        TTF_SetFontKerning(f, (int)kerning);
        if (oldHinting != hinting)
        {
            TTF_SetFontHinting(f, hinting);
        }
        if (outline != oldOutline)
        {
            TTF_SetFontOutline(f, outline);
        }
        if (style != oldStyle)
        {
            TTF_SetFontStyle(f, style);
        }

        SDL_Surface* tempSurface = NULL;

        switch (renderMode)
        {
            case RENDERTEXT_SHADED:
            {
                tempSurface = TTF_RenderUTF8_Shaded(f, text.c_str(), color, bgColor);
                break;
            }
            case RENDERTEXT_BLEND:
            {
                tempSurface = TTF_RenderUTF8_Blended(f, text.c_str(), color);
                break;
            }
            case RENDERTEXT_BLEND_WRAPPED:
            {
                tempSurface = TTF_RenderUTF8_Blended_Wrapped(f, text.c_str(), color, wrapLength);
                break;
            }
            default:
            {
                tempSurface = TTF_RenderUTF8_Solid(f, text.c_str(), color);
                break;
            }
        }

        return tempSurface;
    }

    SDL_Surface* Font::GenerateFromText(Renderer& renderer, string text, const TextStyle& style, Uint32 wrapLength, TTF_Font* f)
    {
        return GenerateFromText(renderer, text, style.fg, style.hinting, style.kerning, style.style, style.rendermode, style.bg, style.outline, wrapLength, f);
    }

    Glyph* Font::GetGlyph(Renderer& renderer, string utfChar, int style, int outline)
    {
        // Limit style to main styles; strike through and underline styles don't need to be cached as they can be drawn dynamically.
        style = style & (TTF_STYLE_BOLD | TTF_STYLE_ITALIC);

        Uint32 utfCodepoint = Utilities::GetCodepointUTF8(utfChar);
        GlyphID id = CreateGlyphID(utfCodepoint, style, outline);

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
                    // Render the main glyph
                    SDL_Surface* created = GenerateFromText(renderer, utfChar, Colors::WHITE, 0, 0, style, RENDERTEXT_BLEND, Colors::BLACK, outline, 0, font);
                    SDL_Surface* renderedGlyph = SDL_CreateRGBSurfaceWithFormat(0, GetAtlasCellSize().x, GetAtlasCellSize().y, 32, created->format->format);
                    if (created != NULL && renderedGlyph != NULL && SDL_BlitSurface(created, NULL, renderedGlyph, NULL) == 0)
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
                            glyph = new Glyph(utfCodepoint, style, outline);
                            //Logger::EngineLog().Verbose("Created new glyph from code point {0}.", codepoint);
                        }

                        // Render mipmaps
                        for (int level = 0; level < mipmapDepth; level++)
                        {
                            SDL_Rect dest = mipOffsets[level + 1];
                            TTF_Font* mipFont = mipmapFonts[level];
                            if (mipFont == NULL)
                            {
                                // TODO?: resort to manual scaling down? At this point we shouldn't be using mipmaps if they can't be generated.
                            }
                            else
                            {
                                SDL_Surface* mipped = GenerateFromText(renderer, utfChar, Colors::WHITE, 0, 0, style, RENDERTEXT_BLEND, Colors::BLACK, outline, 0, mipFont);
                                if (mipped != NULL)
                                {
                                    //Logger::EngineLog().Info("Blitting from {0} onto {1} (surface = {2})", Vector2(mipped->w, mipped->h), dest, Vector2(renderedGlyph->w, renderedGlyph->h));
                                    SDL_BlitSurface(mipped, NULL, renderedGlyph, &dest);
                                    SDL_FreeSurface(mipped);
                                    mipped = NULL;
                                }
                                else
                                {
                                    Logger::EngineLog().Error("Failed to blit mipmap level {0} for glyph character {1} [{2}]!", level, utfChar, utfCodepoint);
                                }
                            }
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
                    if (created != NULL)
                    {
                        SDL_FreeSurface(created);
                        created = NULL;
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
        if (textureCache.Size() >= GetAtlasMaxGlyphs())
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
        //Logger::EngineLog().Info("Atlas cell = {0}", dest);
        // Get the full cell destination so we can clear it
        SDL_Rect cell = dest;
        // TODO: remove padding
        cell.x -= padding;
        cell.y -= padding;
        cell.w += padding * 2;
        cell.h += padding * 2;
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
            // Even better, scale down at glyph creation time instead of here and do it properly (i.e. next closest point size).
            dest.w = min(dest.w, glyph->cached.GetWidth());
            dest.h = min(dest.h, glyph->cached.GetHeight());

            //Logger::EngineLog().Verbose("Rendering glyph {0} to font atlas...", glyph->GetCodePointUTF8());

            // Overwrite whatever was in the atlas cell before
            SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0);
            SDL_RenderFillRect(render, &cell);

            // Render the actual glyph
            glyph->cached.PushGPU(renderer, SDL_TEXTUREACCESS_STREAMING);
            SDL_Rect clip = {0, 0, glyph->cached.GetWidth(), glyph->cached.GetHeight()};
            SDL_RenderCopy(render, glyph->cached.GetTexture(), &clip, &dest);

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

    void Font::Render(Renderer& renderer, SDL_Rect dest, SDL_Rect* clip, SDL_Color color, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        atlas.Render(renderer.GetRendererSDL(), dest, clip, origin, angle, color, blending, flip);
    }

    Vector2 Font::RenderGlyph(Renderer& renderer, Glyph* glyph, Vector2 position, float pointSize, SDL_Color color, bool kerning, bool rtl, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        SDL_Rect dest = {(int)(position.x), (int)(position.y), 0, 0};
        int maxHeight = (int)ceil(GetFontHeight(pointSize));
        if (glyph == nullptr)
        {
            // Invalid glyph, render a box instead
            int boxPadding = maxHeight / 10;
            dest.x += boxPadding;
            dest.w = (maxHeight / 2);
            dest.h = (maxHeight / 3) * 2;
            dest.y += maxHeight / 8;
            SDL_Color oldColor = renderer.GetDrawColor();
            renderer.SetDrawColor(color);
            SDL_RenderDrawRect(renderer.GetRendererSDL(), &dest);
            renderer.SetDrawColor(oldColor);
            return position + Vector2(rtl ? -(boxPadding * 2 + dest.w) : (boxPadding * 2 + dest.w), 0);
        }
        float scale = (pointSize / loadedPointSize);
        int size = round(glyph->cached.GetHeight() * scale);
        dest = {dest.x, dest.y, size, size};
        SDL_Rect clip = glyph->GetClip();
        // TODO: trilinear filtering/alpha blending
        float level = GetMipMapLevel(pointSize, loadedPointSize);
        clip = GetMipMapClip(clip, (int)level);

        Render(renderer, dest, &clip, color, blending, angle, origin, flip);

        size = round(glyph->GetAdvance() * scale);
        return position + Vector2(rtl ? -size : size, 0);
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
        return maxAtlasGlyphs;
    }

    SDL_Point Font::GetAtlasCellSize()
    {
        return cellSize;
    }

    int Font::GetAtlasMipSize(int level)
    {
        return GetMipMapClip({0, 0, 0, 0}, level).w;
    }

    SDL_Rect Font::GetAtlasCell(Uint32 index)
    {
        SDL_Rect rect = {0, 0, 0, 0};
        if (index < 1 || index > GetAtlasMaxGlyphs() || atlas.GetTexture() == NULL)
        {
            Logger::EngineLog().Warning("Failed to get atlas cell for index {0} (note: index must be between 1 and {1} inclusive and atlas must be initialised)", index, GetAtlasMaxGlyphs());
            return rect;
        }
        int linearPosition = ((index - 1) * cellSize.x);
        rect.x = linearPosition % actualTextureSize.x;
        rect.y = (int)(linearPosition / actualTextureSize.x) * cellSize.y;
        rect.w = cellSize.x;
        rect.h = cellSize.y;
        // TODO: remove padding, all glyphs are the same colour in the atlas so there *shouldn't* be bleeding issues.
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

    float Font::GetFontHeight(float pointSize)
    {
        if (pointSize <= 0)
        {
            // TODO: check that this is accurate; suspect there might be a rounding issue for TTF_GetFontHeight at certain point sizes
            return fontHeight;
        }
        return fontHeight * ((float)pointSize / (float)loadedPointSize);
    }

    float Font::GetFontAscent(float pointSize)
    {
        if (pointSize <= 0)
        {
            return fontAscent;
        }
        return fontAscent * ((float)pointSize / (float)loadedPointSize);
    }

    float Font::GetFontDescent(float pointSize)
    {
        if (pointSize <= 0)
        {
            return fontDescent;
        }
        return fontDescent * ((float)pointSize / (float)loadedPointSize);
    }

    float Font::GetUnderlinePosition(float pointSize)
    {
        // This is more or less how SDL_TTF does it internally
        return GetFontAscent(pointSize) - (GetFontDescent(pointSize) / 2.0f) - 1.0f;
    }

    float Font::GetStrikethroughPosition(float pointSize)
    {
        // This is more or less how SDL_TTF does it internally
        return GetFontHeight(pointSize) / 2.0f;
    }

    SDL_Rect Font::GetMipMapClip(SDL_Rect src, int level)
    {
        SDL_Rect mipMapClip = mipOffsets.empty() ? (SDL_Rect){0, 0, 0, 0} : ((unsigned int)level >= mipOffsets.size() ? mipOffsets.back() : mipOffsets[level]);
        mipMapClip.x += src.x;
        mipMapClip.y += src.y;
        return mipMapClip;
    }

    float Font::GetMipMapLevel(float pointSize, float mainPointSize, int level)
    {
        if (pointSize < 0.0f)
        {
            return 0.0f;
        }
        else if (pointSize >= mainPointSize)
        {
            return (float)level + (mainPointSize / pointSize) - 1.0f;
        }
        return GetMipMapLevel(pointSize, mainPointSize * 0.5f, level + 1);
    }

}
