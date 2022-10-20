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
#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>
#include <unordered_map>
extern "C"
{
    #include <SDL_ttf.h>
}

#include "resourcecontroller.h"
#include "image.h"
#include "coremaths.h"
#include "lrucache.h"
#include "../Core/helpermacros.h"

namespace Ossium
{

    namespace Typographic
    {

        enum TextDirection
        {
            LEFT_TO_RIGHT = 0,
            RIGHT_TO_LEFT
        };

        enum TextAlignment
        {
            LEFT_ALIGNED = 0,
            RIGHT_ALIGNED,
            CENTERED,
            JUSTIFIED
        };

    }

    enum TextRenderModes
    {
        RENDERTEXT_SOLID = 0,
        RENDERTEXT_SHADED,
        RENDERTEXT_BLEND,
        RENDERTEXT_BLEND_WRAPPED
    };

    struct StyleText : public Schema<StyleText, 10>
    {
        DECLARE_BASE_SCHEMA(StyleText, 10);

        StyleText(
            std::string font = "",
            int fontSize = 12,
            SDL_Color color = Colors::Black,
            int hint = 0,
            int kern = 0,
            int outlineThickness = 0,
            int styling = 0,
            int renderingMode = RENDERTEXT_BLEND,
            int alignment = Typographic::TextAlignment::LEFT_ALIGNED,
            SDL_Color backgroundColor = Colors::Transparent
        );

        M(std::string, fontPath);
        M(int, ptsize) = 12;
        M(SDL_Color, fg) = Colors::Black;
        M(int, hinting) = 0;
        M(int, kerning) = 0;
        M(int, outline) = 0;
        M(int, style) = 0;
        M(int, rendermode) = RENDERTEXT_SOLID;
        M(int, alignment) = Typographic::TextAlignment::LEFT_ALIGNED;
        M(SDL_Color, bg) = Colors::Transparent;
    };

    // Glyphs are different depending not only on their UTF-8 code point, but also their style and outline thickness in pixels.
    // Because the maximum UTF-8 code point is 0x1FFFFF, we have 11 spare bits to use for style and outline data :D
    typedef Uint32 GlyphID;

    //                                         [hinting]
    //                                            ||[style]
    //                                   [outline]||||[   UTF-8 reserved   ]
    const Uint32 GLYPH_UNICODE_MASK  =   0b00000000000111111111111111111111;

    //                                         [hinting]
    //                                            ||[style]
    //                                   [outline]||||[   UTF-8 reserved   ]
    const Uint32 GLYPH_STYLE_MASK    =   0b00000000011000000000000000000000;
    const Uint32 GLYPH_STYLE_SHIFT   =   21;

    //                                         [hinting]
    //                                            ||[style]
    //                                   [outline]||||[   UTF-8 reserved   ]
    const Uint32 GLYPH_HINTING_MASK  =   0b00000001100000000000000000000000;
    const Uint32 GLYPH_HINTING_SHIFT =   23;

    //                                         [hinting]
    //                                            ||[style]
    //                                   [outline]||||[   UTF-8 reserved   ]
    const Uint32 GLYPH_OUTLINE_MASK  =   0b11111110000000000000000000000000;
    const Uint32 GLYPH_OUTLINE_SHIFT =   25;

    /// Creates a glyph ID
    GlyphID CreateGlyphID(Uint32 codepoint, Uint8 style, Uint8 hinting, Uint8 outline);

    // Forward declaration
    class Font;

    /// Contains meta data about a particular glyph.
    class GlyphMeta
    {
    public:
        /// Default constructor initialises codepoint to zero.
        GlyphMeta();
        /// Constructor loads metrics from a given font
        GlyphMeta(Uint32 codepoint, Font& font, Uint8 style = TTF_STYLE_NORMAL, Uint8 hinting = TTF_HINTING_NORMAL, Uint8 outline = 0);

        /// Computes the difference to the next glyph for a given point size.
        float GetAdvance(float pointSize);

        /// Computes the dimensions of the glyph at a given point size.
        Vector2 GetDimensions(float pointSize);

        /// Returns the advance at the loaded point size.
        float GetAdvance();

        /// Returns the dimensions at the loaded point size.
        Vector2 GetDimensions();

        /// Returns the Unicode codepoint.
        Uint32 GetCodepoint();

    private:
        /// The loaded point size of the font, cached here to account for scaling.
        Uint16 loadedPointSize;

        /// Distance from this glyph's origin to the next glyph origin. Always positive, even for RTL glyphs.
        Uint16 advanceMetric;

        /// The dimensions of the glyph
        Vector2 dimensions;

        /// Unicode codepoint
        Uint32 cp;
    };

    // This is only separate to Font due to Resource class being only allowed base class
    class FontRenderInput : public RenderInput
    {
    public:
        // Return the name of this font for graphics debugging purposes
        std::string GetRenderDebugName();

        // RenderInput override
        void Render();

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
        bool Load(std::string guid_path, int maxPointSize = 96);
        bool LoadAndInit(std::string guid_path, int maxPointSize, Renderer* renderer, Uint32 glyphCacheLimit = 0, int mipDepth = 0, Uint32 targetTextureSize = 0);

        /// Takes a target size for the atlas texture, as well as how much padding there should be per glyph. If mipDepth == 0, automatically computes the mipmap depth based on a minimum point size of 8 points.
        bool Init(std::string guid_path, Renderer* renderer, Uint32 glyphCacheLimit = 0, int mipDepth = 0, Uint32 targetTextureSize = 0);

        /// Renders with a text string from a TrueType font to a single surface on the fly.
        /**
         *  Please note: It is not recommended that you use this method for real-time text rendering as it does not cache glyphs, so it is rather slow.
         *  It does, however, support more out-of-the-box options in regards to text styling, kerning, hinting and so on as well as basic text wrapping.
         */
        SDL_Surface* GenerateFromText(
            std::string text,
            SDL_Color color = Colors::Red,
            int hinting = 0,
            int kerning = 1,
            int style = 0,
            int renderMode = RENDERTEXT_BLEND,
            SDL_Color bgColor = Colors::Black,
            int outline = 0,
            Uint32 wrapLength = 0,
            TTF_Font* f = NULL
        );

        /// Ditto, but bundled some parameters.
        SDL_Surface* GenerateFromText(std::string text, const StyleText& style, Uint32 wrapLength, TTF_Font* f = NULL);

        /// Copies a glyph to the font atlas.
        /** You should call this between BatchPackBegin() and BatchPackEnd(). Returns the number of glyphs that have been packed since BatchPackBegin() was last called.
         *  When the return value == GetAtlasMaxGlyphs(), you should call BatchPackEnd(), otherwise you'll render over other glyphs in the batch.
         *  Note: if this method returns 0, it doesn't necessarily mean there was an error packing the glyph - in circumstances where the glyph is already packed,
         *  the return value does not change. You can check if there was an error packing the glyph by checking glyph->GetAtlasIndex() > 0. */
        Uint32 BatchPackGlyph(GlyphID id);

        /// Returns the total number of glyphs currently batched.
        Uint32 GetBatchPackTotal();

        /// Renders the font atlas texture at a given point size.
        void Render(RenderInput* pass, SDL_Rect dest, SDL_Rect* clip = NULL, SDL_Color color = Colors::Red, SDL_BlendMode blending = SDL_BLENDMODE_BLEND, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Renders a single glyph at the chosen position. Returns true if the glyph is valid and rendered successfully, otherwise renders an invalid glyph box and returns false.
        /// TODO: provide alternate method to render the invalid glyph box
        bool RenderGlyph(
            RenderInput* pass,
            GlyphID id,
            Vector2 position,
            float pointSize,
            SDL_Color color = Colors::Red,
            bool kerning = true,
            Typographic::TextDirection direction = Typographic::TextDirection::LEFT_TO_RIGHT,
            SDL_BlendMode blending = SDL_BLENDMODE_BLEND,
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

        /// Returns the optimal difference between each text line in pixels.
        float GetLineDifference(float pointSize = -1);

        /// Returns the relative Y position for an underline for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetUnderlinePosition(float pointSize = -1.0f);

        /// Returns the relative Y position for a strike-through line for the given point size in pixels.
        /// A negative point size returns the value for the loaded (maximum) point size in pixels.
        float GetStrikethroughPosition(float pointSize = -1.0f);

        /// Returns the point size that was specified when loading the font.
        int GetLoadedPointSize();

        /// Returns the mipmap clip rect for a given source and level.
        SDL_Rect GetMipMapClip(SDL_Rect src, int level);

        /// Returns the mipmap level for a given point size. The decimal part indicates the bias towards the next mipmap level.
        float GetMipMapLevel(float pointSize, float mainPointSize, int level = 0);

        /// Returns the dimensions for invalid glyphs.
        Vector2 GetInvalidGlyphDimensions(float pointSize);

        /// Removes the specified number of glyphs from the atlas texture. If quantity = 0, removes all glyphs from the atlas.
        /**
            Only call this if you know what you're doing, as misuse will slow down text rendering! You've been warned.
            Useful for purging glyphs that haven't been used for a while, as it removes glyphs in order of least recently rendered.
        */
        void ClearAtlas(Uint32 quantity = 0);

    private:
        /// Internal structure for caching a single glyph and storing relevant meta data for the font atlas.
        class Glyph
        {
        public:
            /// The clip rect of the glyph in the font atlas.
            SDL_Rect clip;

            /// Some glyphs are downscaled slightly when packed in the font atlas.
            float inverseScale = 1.0f;

            /// The index of the glyph within the atlas.
            Uint32 atlasIndex = 0;

            /// The glyph image is also cached so it doesn't need to be re-rendered
            /// if it gets removed from the atlas texture.
            Image cached;

            GlyphID id;

        };

        /// Returns the glyph for the given UTF-8 character.
        /** If the glyph is not in the glyphs map already, it first renders the glyph to a surface
         *  then adds it to the map and updates the cache. Note this does NOT pack the glyph into the atlas. */
        Glyph* GetGlyph(GlyphID id);

        /// Internal method for batching a glyph.
        Uint32 BatchPackGlyph(GlyphID id, Glyph* glyph);

        /// Copying is not permitted.
        NOCOPY(Font);

        /// Path to the font so it can be reloaded if necessary
        std::string path;

        /// Pointer to the main font in memory
        TTF_Font* font = NULL;

        /// Pointers to fonts at different point sizes for mipmaps
        std::vector<TTF_Font*> mipmapFonts;

        /// The font atlas texture.
        Image atlas;

        /// The maximum glyph height of the font.
        int fontHeight = 0;

        /// Font ascent.
        int fontAscent = 0;

        /// Font descent.
        int fontDescent = 0;

        /// The vertical difference between each line of text.
        int lineDiff = 0;

        /// The dimensions used for all invalid glyphs (except non-printable ASCII characters), not including the horizontal advance padding.
        Vector2 invalidDimensions;

        /// The horizontal advance padding for invalid glyphs.
        float invalidPadding;

        /// Should the atlas texture be updated?
        bool updateAtlasTexture = true;

        /// The size of an atlas cell, including mipmaps.
        SDL_Point cellSize = {0, 0};

        /// The dimensions of the entire atlas texture.
        SDL_Point actualTextureSize = {0, 0};

        /// The maximum number of mipmaps that should be generated per glyph.
        int mipmapDepth = 0;

        /// The maximum number of glyphs in the texture atlas.
        Uint32 maxAtlasGlyphs = 0;

        /// A very rough limit specifying the *target* maximum number of glyphs that can be packed in the font atlas at any one time.
        /// TODO: rename, this isn't accurate - it's partly dependant on atlas size vs font height, this just acts as a rough limiter.
        const Uint32 ABSOLUTE_MAXIMUM_ATLAS_GLYPHS = 1024;

        /// The relative mipmap rect for each mipmap level.
        std::vector<SDL_Rect> mipOffsets;

        /// Map of IDs to cached glyphs.
        /// TODO?: use slot_map/array instead?
        std::unordered_map<GlyphID, Glyph*> glyphs;

        /// Maps atlas indexes to glyph ids.
        std::map<Uint32, GlyphID> atlasGlyphMap;

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

        // RenderInput instance for creating the atlas
        FontRenderInput texturePass;

    };

}

#endif // FONT_H
