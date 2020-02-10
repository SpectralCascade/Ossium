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
#ifndef TEXTUREPACK_H
#define TEXTUREPACK_H

#include <math.h>

#include <SDL2/SDL_ttf.h>

#include "lrucache.h"
#include "helpermacros.h"
#include "image.h"

using namespace std;

namespace Ossium
{

    /// Contains meta information about the individual textures such as their location in the texture pack.
    struct TextureData : public Schema<TextureData, 3>
    {
        DECLARE_BASE_SCHEMA(TextureData, 3);

        M(string, path);
        M(SDL_Rect, pureClip);
        M(bool, mipmapped);

    };

    /// Data about an individual (not yet packed) texture.
    struct ImportedTextureData : public TextureData
    {
        CONSTRUCT_SCHEMA(SchemaRoot, TextureData);

        ImportedTextureData(string rawPath, SDL_Rect clip, bool hasMipmaps, Image* tex);

        Image* texture;
    };

    struct TexturePackSchema : public Schema<TexturePackSchema, 1>
    {
        DECLARE_BASE_SCHEMA(TexturePackSchema, 1);

        M(vector<TextureData>, packData);

    };

    /// Returns the clip rect for a specified mipmap level of a given source clip rect
    /** Min size is used as the baseline multiplier
     * (e.g. when minSize = 16, level n = 16, level n - 1 = 32, level n - 2 = 64, etc.) **/
    SDL_Rect GetMipMapClipTP(SDL_Rect src, int level = 0, Uint16 minSize = 4);

    class TexturePack : public Resource, public TexturePackSchema
    {
    public:
        DECLARE_RESOURCE(TexturePack);
        CONSTRUCT_SCHEMA(SchemaRoot, TexturePackSchema);

        TexturePack() = default;
        ~TexturePack();

        /// Frees the texture package
        void FreePack();
        /// Frees imported data
        void FreeImported();
        /// Frees everything
        void FreeAll();

        /// Loads a texture pack (JSON meta data + image(s)).
        bool Load(string path);
        /// Initialises the loaded texture
        bool Init(Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN);

        /// Loads up a single texture ready to be packed and generates mip maps for it if required
        /// TODO: mipmaps can be packed more tightly if they are packed as individual textures
        bool Import(string path, Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN, int minMipMapSize = 4);

        /// Imports glyphs from a TrueType font as individual textures ready for packing.
        /** Does not generate mip-maps, but does allow a specified point size and style. */
        bool ImportFont(
            string path,
            int pointSize,
            Renderer& renderer,
            string charset = "!\"$%^&*()[]{}+=-_,./<>?\\|`¬#~'@0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcefghijklmnopqrstuvwxyz",
            int style = TTF_STYLE_NORMAL,
            long index = 0,
            Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN
        );

    private:
        bool CreateFromUTF8(string fontName, int ptSize, string utfChar, TTF_Font* font, Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN, int style = TTF_STYLE_NORMAL);

    public:

        /// Packs all imported textures into one single texture; returns number of textures packed
        /** Smallest first means textures are sorted smallest to largest when true, or largest to smallest when false
          * Max size determines the maximum size of the final texture. **/
        int PackImported(Renderer& renderer, Uint32 pixelFormatting, bool smallestFirst = true, Uint16 maxSize = 4096);

        /// Saves the texture as a .png file and meta data in JSON.
        bool Save(Renderer& renderer, Uint32 pixelFormatting, string path);

        /// Returns a reference to the packed texture
        Image& GetPackedTexture();

        /// Returns a clip rect for a given texture
        SDL_Rect GetClip(string textureId);

        /// Overload returns a mipmap, if the texture has a generated mipmap
        SDL_Rect GetClip(string textureId, int mipmapLevel);

    private:
        NOCOPY(TexturePack);

        /// The texture sheet of the texture pack.
        Image packedTexture;

        /// Imported textures with meta data ready to be packed.
        vector<ImportedTextureData> importedData;

        /// TODO: remove these
        bool compareImportedSmallestFirst(ImportedTextureData& i, ImportedTextureData& j);
        bool compareImportedBiggestFirst(ImportedTextureData& i, ImportedTextureData& j);

    };

}

#endif // TEXTUREPACK_H
