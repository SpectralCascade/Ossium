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
#include "font.h"
#include "logging.h"
#include "utf8.h"

using namespace std;

namespace Ossium
{

    //
    // StyleText
    //

    StyleText::StyleText(string font, int fontSize, SDL_Color color, int hint, int kern, int outlineThickness,
        int styling, int renderingMode, int alignment, SDL_Color backgroundColor)
    {
        fontPath = font;
        ptsize = fontSize;
        fg = color;
        hinting = hint;
        kerning = kern;
        outline = outlineThickness;
        style = styling;
        rendermode = renderingMode;
        this->alignment = alignment;
        bg = backgroundColor;
    }

    //
    // GlyphID
    //

    GlyphID CreateGlyphID(Uint32 codepoint, Uint8 style, Uint8 hinting, Uint8 outline)
    {
        return codepoint | (((Uint32)style << GLYPH_STYLE_SHIFT) & GLYPH_STYLE_MASK) | (((Uint32)hinting << GLYPH_HINTING_SHIFT) & GLYPH_HINTING_MASK) | (((Uint32)outline << GLYPH_OUTLINE_SHIFT) & GLYPH_OUTLINE_MASK);
    }

    //
    // GlyphMeta
    //

    GlyphMeta::GlyphMeta()
    {
        cp = 0;
    }

    GlyphMeta::GlyphMeta(Uint32 codepoint, Font& font, Uint8 style, Uint8 hinting, Uint8 outline)
    {
        cp = codepoint;
        TTF_Font* f = font.GetFont();
        if (f != NULL)
        {
            loadedPointSize = font.GetLoadedPointSize();
            if (TTF_GlyphIsProvided(f, codepoint))
            {
                // Glyph metrics and dimensions will differ depending on style, hinting etc. so set those first
                style &= (TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
                if (TTF_GetFontStyle(f) != (int)style)
                {
                    TTF_SetFontStyle(f, style);
                }
                if (TTF_GetFontHinting(f) != (int)hinting)
                {
                    TTF_SetFontStyle(f, hinting);
                }
                if (TTF_GetFontOutline(f) != (int)outline)
                {
                    TTF_SetFontOutline(f, outline);
                }
                // Get glyph metrics
                int dud, advance, w, h;
                TTF_GlyphMetrics(f, cp, &dud, &dud, &dud, &dud, &advance);
                advanceMetric = advance;
                // TODO: upgrade SDL_TTF and use UCS-4 instead of UCS-2.
                const Uint16 unicode[2] = {(Uint16)codepoint, 0};
                if (TTF_SizeUNICODE(f, unicode, &w, &h) >= 0)
                {
                    dimensions = Vector2(w, h);
                }
                else
                {
                    Log.Warning("Failed to get glyph dimensions for codepoint {0}! TTF_Error: {1}", codepoint, TTF_GetError());
                }
            }
            else
            {
                dimensions = font.GetInvalidGlyphDimensions(loadedPointSize);
                advanceMetric = dimensions.x;
            }
        }
        else
        {
            Log.Error("Failed to get glyph metrics from font, font is not loaded!");
        }
    }

    float GlyphMeta::GetAdvance(float pointSize)
    {
        return (float)advanceMetric * (pointSize / loadedPointSize);
    }

    Vector2 GlyphMeta::GetDimensions(float pointSize)
    {
        float scale = (pointSize / loadedPointSize);
        return Vector2(scale * dimensions.x, scale * dimensions.y);
    }

    float GlyphMeta::GetAdvance()
    {
        return (float)advanceMetric;
    }

    Vector2 GlyphMeta::GetDimensions()
    {
        return dimensions;
    }


    Uint32 GlyphMeta::GetCodepoint()
    {
        return cp;
    }

    //
    // FontRenderInput
    //

    std::string FontRenderInput::GetRenderDebugName()
    {
        return Utilities::Format("Font:{0}", this);
    }

    void FontRenderInput::Render()
    {
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
        Renderer* renderer = texturePass.GetRenderer();
        if (renderer != nullptr)
        {
            //texturePass.GetRenderer()->RemoveInput(&texturePass);
        }
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
            Log.Error("Failed to open font '{0}'! TTF_Error: {1}", guid_path, TTF_GetError());
        }
        return font != NULL;
    }

    bool Font::LoadAndInit(string guid_path, int maxPointSize, Renderer* renderer, Uint32 glyphCacheLimit, int mipDepth, Uint32 targetTextureSize)
    {
        return Load(guid_path, maxPointSize) && Init(guid_path, renderer, glyphCacheLimit, mipDepth, targetTextureSize);
    }

    bool Font::Init(string guid_path, Renderer* renderer, Uint32 glyphCacheLimit, int mipDepth, Uint32 targetTextureSize)
    {
        // TODO enable this but with a different renderer/render target
        // DO NOT ENABLE THIS WITH A WINDOW RENDERER
        //renderer->AddInput(&texturePass);

        updateAtlasTexture = true;
        mipOffsets.clear();

        fontHeight = TTF_FontHeight(font);
        if (fontHeight <= 0)
        {
            Log.Error("Font has invalid height! Cannot initialise.");
            return false;
        }

        // Compute cell size
        cellSize = {fontHeight + (int)ceil((float)fontHeight * 1.5f), fontHeight};

        fontAscent = TTF_FontAscent(font);
        fontDescent = TTF_FontDescent(font);
        lineDiff = max(fontHeight + 1, TTF_FontLineSkip(font));
        invalidDimensions = Vector2((float)fontHeight / 2.0f, ((float)fontHeight / 3.0f) * 2.0f);
        invalidPadding = (float)fontHeight / 10.0f;

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
            //Log.Info("Loading font '{0}' at point size {1}", guid_path, pointSize);
            mipmapFonts.push_back(TTF_OpenFont(guid_path.c_str(), (int)pointSize));
            if (mipmapFonts.back() == NULL)
            {
                Log.Error("TTF_Error opening font '{0}': {1}", guid_path, TTF_GetError());
            }
        }

        if (targetTextureSize == 0)
        {
            // 1024 by default, just in case users have an ancient GPU.
            targetTextureSize = 1024;
        }

        // Compute actual texture size using font height and target texture size
        actualTextureSize = {((int)targetTextureSize / cellSize.x) * cellSize.x, ((int)targetTextureSize / cellSize.y) * cellSize.y};
        maxAtlasGlyphs = (actualTextureSize.x / cellSize.x) * (actualTextureSize.y / cellSize.y);

        if (glyphCacheLimit == 0)
        {
            // Automatically set cache limit based on maximum atlas glyphs.
            cacheLimit = Uint32((float)maxAtlasGlyphs * 1.5f);
        }
        else
        {
            cacheLimit = glyphCacheLimit;
        }
        Log.Verbose("Font {0} has atlas size: {1}, max glyphs: {2}, cache limit: {3}, mipmap depth: {4}", guid_path, actualTextureSize.x, maxAtlasGlyphs, cacheLimit, mipmapDepth);

        // Create the empty atlas surface in transparent white so color modulation works.
        atlas.SetSurface(Image::CreateEmptySurface(actualTextureSize.x, actualTextureSize.y, Alpha(Colors::White, 0)));
        return atlas.GetSurface() != NULL;
    }

    SDL_Surface* Font::GenerateFromText(string text, SDL_Color color, int hinting, int kerning, int style, int renderMode, SDL_Color bgColor, int outline, Uint32 wrapLength, TTF_Font* f)
    {
        if (f == NULL)
        {
            f = font;
            if (f == NULL)
            {
                Log.Error("TTF Font is not loaded for on-the-fly text generation!");
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

    SDL_Surface* Font::GenerateFromText(string text, const StyleText& style, Uint32 wrapLength, TTF_Font* f)
    {
        return GenerateFromText(text, style.fg, style.hinting, style.kerning, style.style, style.rendermode, style.bg, style.outline, wrapLength, f);
    }

    Font::Glyph* Font::GetGlyph(GlyphID id)
    {
        int outline = (id & GLYPH_OUTLINE_MASK) >> GLYPH_OUTLINE_SHIFT;
        int hinting = (id & GLYPH_HINTING_MASK) >> GLYPH_HINTING_SHIFT;
        int style = (id & GLYPH_STYLE_MASK) >> GLYPH_STYLE_SHIFT;

        Uint32 codepoint = id & GLYPH_UNICODE_MASK;

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
            // Check if the glyph exists in the font.
            // TODO: upgrade encoding from UCS-2 to UCS-4 on next SDL_TTF update.
            // Currently the glyphs are limited due to lack of proper character encoding support in SDL_TTF.
            if (TTF_GlyphIsProvided(font, (Uint16)codepoint))
            {
                // Configure font
                if (hinting != TTF_GetFontHinting(font))
                {
                    TTF_SetFontHinting(font, hinting);
                }
                if (outline != TTF_GetFontOutline(font))
                {
                    TTF_SetFontOutline(font, outline);
                }
                if (style != TTF_GetFontStyle(font))
                {
                    TTF_SetFontStyle(font, style);
                }
                // Render the glyph
                // TODO: ditto regarding converting encoding from UCS-2 to UCS-4 when SDL_TTF gets updated
                SDL_Surface* renderedGlyph = TTF_RenderGlyph_Blended(font, (Uint16)codepoint, Colors::White);
                SDL_Surface* created = Image::CreateEmptySurface(GetAtlasCellSize().x, GetAtlasCellSize().y, Alpha(Colors::White, 0));
                if (created != NULL && renderedGlyph != NULL)
                {
                    // First, get the scaling correct
                    float inverseScale = 1.0f;
                    int maxSize = GetAtlasMipSize(0);
                    if (renderedGlyph->w > maxSize)
                    {
                        inverseScale = (float)renderedGlyph->w / (float)maxSize;
                    }
                    if (renderedGlyph->h > GetAtlasMipSize(0))
                    {
                        float rescaled = (float)renderedGlyph->h / (float)maxSize;
                        if (rescaled < inverseScale)
                        {
                            inverseScale = rescaled;
                        }
                    }

                    bool blitSuccess = false;
                    if (inverseScale != 1.0f)
                    {
                        Log.Verbose("Glyph [Codepoint: {0}] is too large for atlas cell, downscaling. Scaling artifacts may be present. Upscale factor is {1}.", codepoint, inverseScale);
                        // Scale the glyph when blitting it
                        float scale = (1.0f / inverseScale);
                        SDL_Rect dest = {0, 0, (int)((float)renderedGlyph->w * scale), (int)((float)renderedGlyph->h * scale)};
                        blitSuccess = SDL_BlitScaled(renderedGlyph, NULL, created, &dest) == 0;
                    }
                    else
                    {
                        blitSuccess = SDL_BlitSurface(renderedGlyph, NULL, created, NULL) == 0;
                    }

                    if (blitSuccess)
                    {
                        // Update the cache
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
                                Log.Error("Font system failure! LRU decimal code point {0} not found in glyphs map :(", toReplace);
                            }
                        }
                        else
                        {
                            // Create a new glyph
                            glyph = new Glyph();
                        }

                        // Render mipmaps
                        for (int level = 0; level < mipmapDepth; level++)
                        {
                            SDL_Rect dest = mipOffsets[level + 1];
                            TTF_Font* mipFont = mipmapFonts[level];
                            if (hinting != TTF_GetFontHinting(mipFont))
                            {
                                TTF_SetFontHinting(mipFont, hinting);
                            }
                            if (outline != TTF_GetFontOutline(mipFont))
                            {
                                TTF_SetFontOutline(mipFont, outline);
                            }
                            if (style != TTF_GetFontStyle(mipFont))
                            {
                                TTF_SetFontStyle(mipFont, style);
                            }
                            if (mipFont == NULL)
                            {
                                // TODO?: resort to manual scaling down? At this point we shouldn't be using mipmaps if they can't be generated.
                            }
                            else
                            {
                                // TODO: downscale mips that are too large
                                SDL_Surface* mipped = TTF_RenderGlyph_Blended(mipFont, codepoint, Colors::White);
                                if (mipped != NULL)
                                {
                                    //Log.Info("Blitting from {0} onto {1} (surface = {2})", Vector2(mipped->w, mipped->h), dest, Vector2(renderedGlyph->w, renderedGlyph->h));
                                    SDL_BlitSurface(mipped, NULL, created, &dest);
                                    SDL_FreeSurface(mipped);
                                    mipped = NULL;
                                }
                                else
                                {
                                    Log.Error("Failed to blit mipmap level {0} for glyph [Codepoint {1}]!", level, codepoint);
                                }
                            }
                        }

                        // Glyph manages surface memory now
                        glyph->cached.SetSurface(created);
                        glyph->inverseScale = inverseScale;
                        glyph->id = id;
                    }
                    else
                    {
                        Log.Error("Failed to blit glyph. SDL_Error: {0}", SDL_GetError());
                    }

                }
                else
                {
                    Log.Error("Failed to render glyph to surface. TTF_Error: {0}", TTF_GetError());
                }
                if (renderedGlyph != NULL)
                {
                    SDL_FreeSurface(renderedGlyph);
                    renderedGlyph = NULL;
                }
            }
            else
            {
                Log.Verbose("Glyph is not provided for UTF-8 character [Codepoint {0}].", codepoint);
            }

        }

        // Add the glyph to the map and update the LRU cache
        glyphs[id] = glyph;
        glyphCache.Access(id);

        return glyph;
    }

    // Private method
    Uint32 Font::BatchPackGlyph(GlyphID id, Glyph* glyph)
    {
        if (!updateAtlasTexture)
        {
            // Start a new batch
            batched = 0;
        }

        if (glyph == nullptr)
        {
            // Early out
            return batched;
        }

        // First check if the glyph is already packed
        Uint32 index = glyph->atlasIndex;
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
                        glyphItr->second->atlasIndex = 0;
                    }
                    failedReplace = false;
                }
            }
            if (failedReplace)
            {
                Log.Error("Failed to find mapped glyph for atlas index {0}! Either the cache is smaller than GetAtlasMaxGlyphs() or this font is not initialised.", index);
            }
        }
        else
        {
            // Use the next available index (must be non-zero, hence +1).
            index = textureCache.Size() + 1;
        }

        // Update atlas cache
        textureCache.Access(index);
        atlasGlyphMap[index] = id;

        SDL_Rect dest = GetAtlasCell(index);

        if (dest.w != 0 && dest.h != 0)
        {
            // Render the glyph to the atlas.
            if (glyph->cached.GetWidth() > dest.w || glyph->cached.GetHeight() > dest.h)
            {
                Log.Warning("Attempting to pack glyph that is bigger than the atlas cell size! It'll be a bit squashed.");
            }
            // For the sake of robustness, don't want to overwrite any other glyphs!
            dest.w = min(dest.w, glyph->cached.GetWidth());
            dest.h = min(dest.h, glyph->cached.GetHeight());

            //Log.Verbose("Rendering glyph {0} to font atlas...", glyph->GetCodePointUTF8());

            // Erase any existing glyph.
            SDL_FillRect(atlas.GetSurface(), &dest, SDL_MapRGBA(atlas.GetSurface()->format, 255, 255, 255, 0));

            // Blit the glyph onto the font atlas
            SDL_Rect src = {0, 0, glyph->cached.GetWidthSurface(), glyph->cached.GetHeightSurface()};
            SDL_BlitScaled(glyph->cached.GetSurface(), &src, atlas.GetSurface(), &dest);

            // Update atlas meta in the glyph
            glyph->atlasIndex = index;
            glyph->clip = dest;

            batched++;
            updateAtlasTexture = true;
        }

        return batched;
    }

    // Public overload
    Uint32 Font::BatchPackGlyph(GlyphID id)
    {
        return BatchPackGlyph(id, GetGlyph(id));
    }

    Uint32 Font::GetBatchPackTotal()
    {
        return batched;
    }

    void Font::Render(RenderInput* pass, SDL_Rect dest, SDL_Rect* clip, SDL_Color color, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        if (updateAtlasTexture)
        {
            atlas.PushGPU(BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
            updateAtlasTexture = false;
        }
        // TODO fix me, should render to target texture NOT window
        atlas.Render(pass, dest, clip, origin, angle, color, blending, flip);
    }

    bool Font::RenderGlyph(RenderInput* pass, GlyphID id, Vector2 position, float pointSize, SDL_Color color, bool kerning, Typographic::TextDirection direction, SDL_BlendMode blending, double angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        Renderer* renderer = pass->GetRenderer();
        Glyph* glyph = GetGlyph(id);
        SDL_Rect dest = {(int)(position.x), (int)(position.y), 0, 0};
        float scale = (pointSize / loadedPointSize);
        if (glyph == nullptr)
        {
            // Invalid glyph, render a box instead
            dest.x += invalidPadding * scale;
            dest.w = invalidDimensions.x * scale;
            dest.h = invalidDimensions.y * scale;
            dest.y += invalidPadding * scale;
            SDL_Color oldColor = renderer->GetDrawColor();
            Rect(dest).Draw(pass, color);
            renderer->SetDrawColor(oldColor);
            return false;
        }
        int size = round((float)glyph->cached.GetHeight() * scale * glyph->inverseScale);
        dest = {dest.x, dest.y, size, size};

        // If glyph is not already in the atlas, pack it now. Note this is less efficient than batch packing multiple glyphs at once.
        if (glyph->atlasIndex == 0)
        {
            BatchPackGlyph(id, glyph);
        }

        // Get correct mipmap level
        SDL_Rect clip = glyph->clip;
        float level = GetMipMapLevel(pointSize, loadedPointSize);
        clip = GetMipMapClip(clip, (int)level);

        // Render the glyph
        Render(pass, dest, &clip, color, blending, angle, origin, flip);

        return true;
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
        atlasGlyphMap.clear();
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
        if (index < 1 || index > GetAtlasMaxGlyphs() || atlas.GetSurface() == NULL)
        {
            Log.Warning("Failed to get atlas cell for index {0} (note: index must be between 1 and {1} inclusive and atlas must be initialised)", index, GetAtlasMaxGlyphs());
            return rect;
        }
        int linearPosition = ((index - 1) * cellSize.x);
        rect.x = linearPosition % actualTextureSize.x;
        rect.y = (int)(linearPosition / actualTextureSize.x) * cellSize.y;
        rect.w = cellSize.x;
        rect.h = cellSize.y;
        return rect;
    }

    float Font::GetFontHeight(float pointSize)
    {
        if (pointSize <= 0)
        {
            // TODO: check that this is accurate; suspect there might be a rounding issue for TTF_GetFontHeight at certain point sizes
            return fontHeight;
        }
        return fontHeight * (pointSize / (float)loadedPointSize);
    }

    float Font::GetFontAscent(float pointSize)
    {
        if (pointSize <= 0)
        {
            return fontAscent;
        }
        return fontAscent * (pointSize / (float)loadedPointSize);
    }

    float Font::GetFontDescent(float pointSize)
    {
        if (pointSize <= 0)
        {
            return fontDescent;
        }
        return fontDescent * (pointSize / (float)loadedPointSize);
    }

    float Font::GetLineDifference(float pointSize)
    {
        if (pointSize <= 0)
        {
            return lineDiff;
        }
        return lineDiff * (pointSize / (float)loadedPointSize);
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

    int Font::GetLoadedPointSize()
    {
        return loadedPointSize;
    }

    SDL_Rect Font::GetMipMapClip(SDL_Rect src, int level)
    {
        if (level < 0)
        {
            level = 0;
        }
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
            return (float)level + (mainPointSize / (pointSize * 0.5f)) - 2.0f;
        }
        return GetMipMapLevel(pointSize, mainPointSize * 0.5f, level + 1);
    }

    Vector2 Font::GetInvalidGlyphDimensions(float pointSize)
    {
        return Vector2(invalidDimensions.x + (invalidPadding * 2.0f), invalidDimensions.y) * (pointSize / (float)loadedPointSize);
    }

    void Font::ClearAtlas(Uint32 quantity)
    {
        if (quantity == 0)
        {
            quantity = maxAtlasGlyphs;
        }
        for (Uint32 i = 0, counti = min(quantity, (Uint32)(atlasGlyphMap.empty() ? 0 : atlasGlyphMap.size())); i < counti; i++)
        {
            Uint32 index = textureCache.GetLRU();
            auto id = atlasGlyphMap.find(index);
            if (id != atlasGlyphMap.end())
            {
                auto found = glyphs.find(id->second);
                if (found != glyphs.end() && found->second != nullptr)
                {
                    // Remove the glyph from the atlas
                    found->second->atlasIndex = 0;
                }
            }
            textureCache.PopLRU();
            atlasGlyphMap.erase(id);
        }
    }

}
