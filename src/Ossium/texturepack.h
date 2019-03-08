#ifndef TEXTUREPACK_H
#define TEXTUREPACK_H

#include <string>
#include <vector>
#include <math.h>

#include "helpermacros.h"
#include "texture.h"

using namespace std;

namespace Ossium
{

    // Data about an individual texture in the TexturePack
    struct TextureData
    {
        string path;
        SDL_Rect pureClip;
        bool mipmapped;
    };

    // Data about an individual imported texture
    struct ImportedTextureData
    {
        string path;
        SDL_Rect pureClip;
        bool mipmapped;
        Image* texture;
    };

    // Returns the clip rect for a specified mipmap level of a given source clip rect
    // Min size is used as the baseline multiplier
    // (e.g. when minSize = 16, level n = 16, level n - 1 = 32, level n - 2 = 64, etc.)
    SDL_Rect getMipMapClip(SDL_Rect src, int level = 0, Uint16 minSize = 4);

    class TexturePack
    {
    public:
        TexturePack();
        ~TexturePack();

        // Frees the texture package
        void freePack();
        // Frees imported data
        void freeImported();
        // Frees everything
        void freeAll();

        // TexturePack is a resource, so it needs these methods for loading and initialising
        // Loads a texture pack (image + meta data)
        bool load(string path, int* args = NULL);
        // Initialises the texture
        bool init(Renderer& renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN);

        // Loads up a single texture ready to be packed and generates mip maps for it if required
        bool import(string path, Renderer& renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN, int minMipMapSize = 4);

        // Packs all imported textures into one single texture; returns number of textures packed
        // Smallest first means textures are sorted smallest to largest when true, or largest to smallest when false
        // Max size determines the maximum size of the final texture
        int packImported(Renderer& renderer, Uint32 windowPixelFormat, bool smallestFirst = true, Uint16 maxSize = 4096);

        // Saves the texture and package data to a PNG format file
        bool save(Renderer& renderer, Uint32 windowPixelFormat, string path);

        // Returns a reference to the packed texture
        Image& getPackedTexture();

        // Returns a clip rect for a given texture
        SDL_Rect getClip(string textureId);
        // Overload returns a mipmap, if the texture has a generated mipmap
        SDL_Rect getClip(string textureId, int mipmapLevel);

    private:
        NOCOPY(TexturePack);

        // The texture sheet of the texture pack
        Image packedTexture;

        // Meta data for the texture pack
        vector<TextureData> packData;
        // Imported textures with meta data
        vector<ImportedTextureData> importedData;

        bool compareImportedSmallestFirst(ImportedTextureData& i, ImportedTextureData& j);
        bool compareImportedBiggestFirst(ImportedTextureData& i, ImportedTextureData& j);

    };

}

#endif // TEXTUREPACK_H