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
#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>
#include <unordered_map>
extern "C"
{
    #include <SDL2/SDL_ttf.h>
}

#include "resourcecontroller.h"
#include "image.h"
#include "coremaths.h"
#include "lrucache.h"
#include "../Core/helpermacros.h"

using namespace std;

namespace Ossium
{

    enum TextRenderModes
    {
        RENDERTEXT_SOLID = 0,
        RENDERTEXT_SHADED,
        RENDERTEXT_BLEND,
        RENDERTEXT_BLEND_WRAPPED
    };

    struct TextStyle : public Schema<TextStyle, 9>
    {
        DECLARE_BASE_SCHEMA(TextStyle, 9);

        TextStyle(
            string font = "",
            int fontSize = 12,
            SDL_Color color = Colors::BLACK,
            int hint = 0,
            int kern = 0,
            int outlineThickness = 0,
            int styling = 0,
            int renderingMode = RENDERTEXT_BLEND,
            SDL_Color backgroundColor = Colors::TRANSPARENT
        );

        M(string, fontPath);
        M(int, ptsize) = 12;
        M(SDL_Color, fg) = Colors::BLACK;
        M(int, hinting) = 0;
        M(int, kerning) = 0;
        M(int, outline) = 0;
        M(int, style) = 0;
        M(int, rendermode) = RENDERTEXT_SOLID;
        M(SDL_Color, bg) = Colors::TRANSPARENT;
    };

    // Glyphs are different depending not only on their UTF-8 code point, but also their style and outline thickness in pixels.
    // Because the maximum UTF-8 code point is 0x1FFFFF, we have 11 spare bits to use for style and outline data :D
    typedef Uint32 GlyphID;

    //                                                  [style]
    //                                          [outline]||||[   UTF-8 reserved   ]
    const static Uint32 GLYPH_UTF8_MASK     =   0b00000000000111111111111111111111;

    //                                                  [style]
    //                                          [outline]||||[   UTF-8 reserved   ]
    const static Uint32 GLYPH_STYLE_MASK    =   0b00000001111000000000000000000000;
    const static Uint32 GLYPH_STYLE_SHIFT   =   21;

    //                                          [style]
    //                                          [outline]||||[   UTF-8 reserved   ]
    const static Uint32 GLYPH_OUTLINE_MASK  =   0b11111110000000000000000000000000;
    const static Uint32 GLYPH_OUTLINE_SHIFT =   25;

    /// Creates a glyph ID
    static inline GlyphID CreateGlyphID(Uint32 codepoint, Uint32 style, Uint32 outline)
    {
        return codepoint | (style << GLYPH_STYLE_SHIFT) | (outline << GLYPH_OUTLINE_SHIFT);
    }

    // Forward declaration
    class Font;

    class Glyph
    {
    public:
        friend class Font;

        Glyph(Uint32 codepoint, Uint32 style, Uint32 outline);

        /// Updates the atlas index and clip rect
        void UpdateMeta(Uint32 index, SDL_Rect quad, float inverseScaling);

        /// Returns the clip rect of the glyph in the texture atlas.
        SDL_Rect GetClip();

        /// Returns the index of the glyph in the texture atlas.
        /// Returns 0 if not packed.
        Uint32 GetAtlasIndex();

        /// Returns the bounding box of the rendered glyph.
        SDL_Rect GetBoundingBox();

        /// Returns the pixel advance to the next glyph origin.
        int GetAdvance();

        /// Returns the UTF-8 code point
        Uint32 GetCodePointUTF8();

        /// Returns glyph type information (UTF-8 code point, style, outline thickness in pixels).
        GlyphID GetID();

        /// Some glyphs cannot fit in font atlas cell unless downscaled, so this is what the scale factor is for.
        float GetInverseScaleFactor();

    private:
        /// The clip rect of the glyph in the font atlas.
        SDL_Rect clip;

        /// The rendered glyph's bounding box
        SDL_Rect bbox;

        /// Distance from this glyph's origin to the next glyph origin. Always positive, even for RTL glyphs.
        int advanceMetric;

        /// Some glyphs are downscaled slightly
        float inverseScale = 1.0f;

        /// The index of the glyph within the atlas.
        Uint32 atlasIndex = 0;

        /// The unique identifier for this rendered glyph, consisting of the UTF-8 code point, render style and outline thickness.
        GlyphID type;

        /// The glyph image is also cached so it doesn't need to be re-rendered
        /// if it gets removed from the atlas texture.
        Image cached;

    };

    class OSSIUM_EDL Font : public Resource
    {
    public:
        DECLARE_RESOURCE(Font);

        Font();
        ~Font();

        /// Frees everything.
        void Free();
        /// Frees the texture atlas from GPU memory.
        void FreeAtlas();

        /// Loads a TrueType Font at the specified point size. Lower point sizes are rendered by downscaling this point size with mip maps.
        bool Load(string guid_path, int maxPointSize = 96);
        bool LoadAndInit(string guid_path, int maxPointSize, Renderer& renderer, Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888, Uint32 glyphCacheLimit = 256, int mipDepth = 0, Uint32 targetTextureSize = 0);

        /// Takes a target size for the atlas texture, as well as how much padding there should be per glyph. If mipDepth == 0, automatically computes the mipmap depth based on a minimum point size of 8 points.
        bool Init(string guid_path, Renderer& renderer, Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888, Uint32 glyphCacheLimit = 256, int mipDepth = 0, Uint32 targetTextureSize = 0);

        /// Renders with a text string from a TrueType font to a single surface on the fly.
        /**
         *  Please note: It is not recommended that you use this method for real-time text rendering as it does not cache glyphs, so it is rather slow.
         *  It does, however, support more out-of-the-box options in regards to text styling, kerning, hinting and so on as well as basic text wrapping.
         */
        SDL_Surface* GenerateFromText(
            Renderer& renderer,
            string text,
            SDL_Color color = Colors::RED,
            int hinting = 0,
            int kerning = 1,
            int style = 0,
            int renderMode = RENDERTEXT_BLEND,
            SDL_Color bgColor = Colors::BLACK,
            int outline = 0,
            Uint32 wrapLength = 0,
            TTF_Font* f = NULL
        );

        /// Ditto, but bundled some parameters.
        SDL_Surface* GenerateFromText(Renderer& renderer, string text, const TextStyle& style, Uint32 wrapLength, TTF_Font* f = NULL);

        /// Returns the glyph for the given UTF-8 character.
        /** If the glyph is not in the glyphs map already, it first renders the glyph to a surface
         *  then adds it to the map and updates the cache. Note this does NOT pack the glyph into the atlas. */
        Glyph* GetGlyph(Renderer& renderer, string utf8char, int style = TTF_STYLE_NORMAL, int outline = 0);

        /// TODO: add support for alternative packing mode that doesn't use render targets (maybe cache the texture surface and modify that instead).
        /// Targets the texture atlas for rendering. Returns the original render target.
        void BatchPackBegin(Renderer& renderer);

        /// Pre-renders a glyph to the font atlas but doesn't call RenderPresent().
        /** You should call this between BatchPackBegin() and BatchPackEnd(). Returns the number of glyphs that have been packed since BatchPackBegin() was last called.
         *  When the return value == GetAtlasMaxGlyphs(), you should call BatchPackEnd(), otherwise you'll render over other glyphs in the batch.
         *  Note: if this method returns 0, it doesn't necessarily mean there was an error packing the glyph - in circumstances where the glyph is already packed,
         *  the return value does not change. You can check if there was an error packing the glyph by checking glyph->GetAtlasIndex() > 0. */
        Uint32 BatchPackGlyph(Renderer& renderer, Glyph* glyph);

        /// Renders any outstanding pack batch and resets the renderer to it's configuration before batching.
        void BatchPackEnd(Renderer& renderer);

        /// Returns the total number of glyphs currently batched.
        Uint32 GetBatchPackTotal();

        /// Returns true when glyph batch packing is in progress.
        bool IsBatchPacking();

        /// Renders the font atlas texture at a given point size.
        void Render(Renderer& renderer, SDL_Rect dest, SDL_Rect* clip = NULL, SDL_Color color = Colors::RED, SDL_BlendMode blending = SDL_BLENDMODE_BLEND, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Renders a single glyph at the chosen position, returning the ideal position for the next glyph to be rendered from (for a line of text).
        /// mipBias indicates how mipmap blending (trilinear filtering) should be applied. A value <= 0.0f doesn't apply any mipmap blending, 0.5f applies even mipmap blending,
        /// and 1.0f or above uses the larger mipmap instead.
        Vector2 RenderGlyph(
            Renderer& renderer,
            Glyph* glyph,
            Vector2 position,
            float pointSize,
            SDL_Color color = Colors::RED,
            bool kerning = true,
            bool rtl = false,
            SDL_BlendMode blending = SDL_BLENDMODE_BLEND,
            float mipBias = 0.5f,
            double angle = 0.0,
            SDL_Point* origin = NULL,
            SDL_RendererFlip flip = SDL_FLIP_NONE
        );

        /// Frees all glyphs in the map and clears the LRU caches. Does not destroy the atlas texture.
        void FreeGlyphs();

        /// Returns pointer to a font. Useful if you want to use SDL_ttf functions directly.
        TTF_Font* GetFont();

        /// Returns the size of the atlas (pixel width and height, which are the same as it's a square).
        Uint32 GetAtlasSize();

        /// Returns the maximum number of glyphs that can fit in the atlas.
        Uint32 GetAtlasMaxGlyphs();

        /// Returns the dimensions of a single glyph cell in the atlas, including mipmaps.
        SDL_Point GetAtlasCellSize();

        /// Returns the size of a mipmap in the atlas at the specified level. Note that width and height are the same for individual mipmaps.
        int GetAtlasMipSize(int level);

        /// Returns the clip rect for a cell within the atlas, with optional padding. Returns rect with 0 width and height on error (e.g. index out of range).
        SDL_Rect GetAtlasCell(Uint32 index);

        /// Returns the maximum font height for a given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetFontHeight(float pointSize = -1);

        /// Returns the font ascent for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetFontAscent(float pointSize = -1);

        /// Returns the font descent for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetFontDescent(float pointSize = -1);

        /// Returns the relative Y position for an underline for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetUnderlinePosition(float pointSize = -1.0f);

        /// Returns the relative Y position for a strike-through line for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetStrikethroughPosition(float pointSize = -1.0f);

        /// Returns the mipmap clip rect for a given source and level.
        SDL_Rect GetMipMapClip(SDL_Rect src, int level);

        /// Returns the mipmap level for a given point size. The decimal part indicates the bias towards the next mipmap level.
        float GetMipMapLevel(float pointSize, float mainPointSize, int level = 0);

    private:
        /// Copying is not permitted.
        Font(const Font& thisCopy);
        Font operator=(const Font& thisCopy);

        /// Path to the font so it can be reloaded if necessary
        string path;

        /// Pointer to the main font in memory
        TTF_Font* font = NULL;

        /// Pointers to fonts at different point sizes for mipmaps
        vector<TTF_Font*> mipmapFonts;

        /// The font atlas texture.
        Image atlas;

        /// The maximum glyph height of the font.
        int fontHeight = 0;

        /// Font ascent.
        int fontAscent = 0;

        /// Font descent.
        int fontDescent = 0;

        /// The size of an atlas cell, including mipmaps.
        SDL_Point cellSize = {0, 0};

        /// The dimensions of the entire atlas texture.
        SDL_Point actualTextureSize = {0, 0};

        /// The maximum number of mipmaps that should be generated per glyph.
        int mipmapDepth = 0;

        /// The maximum number of glyphs in the texture atlas.
        Uint32 maxAtlasGlyphs = 0;

        /// The relative mipmap rect for each mipmap level.
        vector<SDL_Rect> mipOffsets;

        /// Map of IDs to cached glyphs.
        /// TODO?: use slot_map/array instead?
        unordered_map<GlyphID, Glyph*> glyphs;

        /// Maps atlas indexes to glyph ids.
        map<Uint32, GlyphID> atlasGlyphMap;

        /// Keeps track of the least recently used glyph in the texture atlas.
        LRUCache<Uint32> textureCache;

        /// Ditto but for the entire glyphs map, not just the texture atlas (as the cache can be larger than the atlas).
        LRUCache<GlyphID> glyphCache;

        /// The loaded point size
        int loadedPointSize = 0;

        /// How many glyphs can be cached in RAM (per glyph map) at a time? This may be important to consider when dealing with large character sets (e.g. Traditional Chinese).
        /** The lower this is, the less memory is used, but performance could drop if you're using a large number of unique glyphs. */
        Uint32 cacheLimit = 0;

        /// The number of glyphs that have been batched so far. This is reset to zero when BatchPackBegin() or BatchPackEnd() are called.
        Uint32 batched = 0;

        /// The renderer target texture when batch packing begins
        SDL_Texture* originalTarget = NULL;

        /// The renderer blending mode when batch packing begins. When invalid, batching is not in progress.
        SDL_BlendMode originalBlending = SDL_BLENDMODE_INVALID;

    };

}

#endif // FONT_H
