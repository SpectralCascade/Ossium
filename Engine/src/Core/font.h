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
#include <unordered_set>
#include <stack>
extern "C"
{
    #include <SDL_ttf.h>
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

    // Forward declaration
    class Font;

    class Glyph
    {
    public:
        friend class Font;

        Glyph(Uint32 codepoint);

        /// Updates the atlas index and clip rect
        void UpdateMeta(Uint32 index, SDL_Rect quad);

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

    private:
        /// The clip rect of the glyph in the font atlas.
        SDL_Rect clip;

        /// The rendered glyph's bounding box
        SDL_Rect bbox;

        /// Distance from this glyph's origin to the next glyph origin. Always positive, even for RTL glyphs.
        int advanceMetric;

        /// The index of the glyph within the atlas.
        Uint32 atlasIndex = 0;

        /// UTF-8 code point
        Uint32 cp;

        /// The glyph image is also cached so it doesn't need to be re-rendered
        /// if it gets removed from the atlas texture.
        Image cached;

    };

    /// Helper class for batching glyphs rendered to the font atlas.
    /** Rather than rendering a glyph to the atlas, then immediately rendering the atlas, this is used to render a batch of glyphs
     *  to the atlas if they're not already in the atlas. Then you can render the glyphs from the atlas texture all at once when the batch is full
     *  (or you don't need to add any more glyphs). */
    class GlyphBatch
    {
    public:
        /// Takes the maximum glyphs that can fit in the atlas (maxGlyphs).
        GlyphBatch(int atlasLimit);

        /// Adds a glyph to the batch. Returns true when the batch is full, at which point all the glyphs should be rendered.
        bool AddGlyph(Glyph* glyph);

        /// Removes and returns a glyph from the batch.
        Glyph* PopGlyph();

        /// Is this batch full?
        bool IsFull();

        /// Clears the batch.
        void Clear();

    private:
        /// Atlas glyph limit
        int maxGlyphs;

        /// Stack of glyphs that expands as more are added to the batch.
        stack<Glyph*> glyphs;

        /// When the size of this set == maxGlyphs, the batch is full and should be emptied.
        unordered_set<Glyph*> batched;

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
        bool LoadAndInit(string guid_path, Renderer& renderer, int maxPointSize = 96, Uint16 targetTextureSize = 1024, Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888, Uint32 glyphCacheLimit = 256);

        /// Takes a target size for the atlas textures
        bool Init(string guid_path, Renderer& renderer, Uint16 targetTextureSize = 1024, Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888, Uint32 glyphCacheLimit = 256);

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
            int kerning = 0,
            int style = 0,
            int renderMode = 0,
            SDL_Color bgColor = Colors::BLACK,
            int outline = 0,
            Uint32 wrapLength = 0
        );

        /// Ditto, but bundled some parameters.
        SDL_Surface* GenerateFromText(Renderer& renderer, string text, const TextStyle& style, Uint32 wrapLength);

        /// Returns the glyph for the given UTF-8 character.
        /** If the glyph is not in the glyphs map already, it first renders the glyph to a surface
         *  then adds it to the map and updates the cache. Note this does NOT pack the glyph into the atlas. */
        Glyph* GetGlyph(Renderer& renderer, string utf8char, int style = TTF_STYLE_NORMAL);

        /// Packs a given glyph into the font atlas if it isn't already packed. Returns the number of glyphs that were successfully packed.
        /// Note that if the size of renderGlyphs is greater than GetAtlasMaxGlyphs() then only the first GetAtlasMaxGlyphs() glyphs will be packed.
        Uint32 PackGlyphs(Renderer& renderer, vector<Glyph*> renderGlyphs);

        /// Renders the font atlas texture at a given point size.
        void Render(Renderer& renderer, SDL_Rect dest, SDL_Rect clip, SDL_Color color = Colors::RED, SDL_BlendMode blending = SDL_BLENDMODE_BLEND, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Renders a single glyph at the chosen position, returning the ideal position for the next glyph to be rendered from (for a line of text).
        Vector2 RenderGlyph(Renderer& renderer, Glyph* glyph, Vector2 position, float pointSize, int style, SDL_Color color = Colors::RED, bool kerning = true, bool rtl = false, SDL_BlendMode blending = SDL_BLENDMODE_BLEND, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Frees all glyphs in the map and clears the LRU caches. Does not destroy the atlas texture.
        void FreeGlyphs();

        /// Returns pointer to a font. Useful if you want to use SDL_ttf functions directly.
        TTF_Font* GetFont();

        /// Returns the size of the atlas (pixel width and height, which are the same as it's a square).
        Uint32 GetAtlasSize();

        /// Returns the maximum number of glyphs that can fit in the atlas.
        Uint32 GetAtlasMaxGlyphs();

        /// Returns the size of a single glyph cell in the atlas.
        int GetAtlasCellSize();

        /// Returns the clip rect for a cell within the atlas, with optional padding. Returns rect with 0 width and height on error (e.g. index out of range).
        SDL_Rect GetAtlasCell(Uint32 index, int padding = 0);

    private:
        /// Copying is not permitted.
        Font(const Font& thisCopy);
        Font operator=(const Font& thisCopy);

        /// Path to the font so it can be reloaded if necessary
        string path;

        /// Pointer to the main font in memory
        TTF_Font* font = NULL;

        /// The font atlas texture.
        Image atlas;

        /// The maximum glyph height of the font
        int fontHeight;

        /// Map of UTF-8 code points to cached glyphs.
        /// TODO?: use slot_map/array instead?
        map<Uint32, Glyph*> glyphs;

        /// Keeps track of the least recently used UTF-8 code point in the texture atlas.
        LRUCache<Uint32> textureCache;

        /// Ditto but for the entire glyphs map, not just the texture atlas.
        LRUCache<Uint32> glyphCache;

        /// The loaded point size
        int loadedPointSize = 0;

        /// How many glyphs can be cached in RAM (per glyph map) at a time? This may be important to consider when dealing with large character sets (e.g. Traditional Chinese).
        /** The total theoretical number of glyphs that could be cached is cacheLimit * FS_TOTAL, though in practice this is likely to be way lower for video games.
         *  The lower this is, the less memory is used. */
        Uint32 cacheLimit;

    };

}

#endif // FONT_H
