#ifndef FONTPACK_H
#define FONTPACK_H

#include "texturepack.h"

namespace Ossium
{

    /// A dynamic font container that caches the bitmaps it generates from TrueType fonts.
    class FontPack
    {
    private:
        /// Map of font point sizes to corresponding texture packs
        map<int, TexturePack*> sizeMap;

    public:
        DECLARE_RESOURCE(FontPack);

        bool Load(string guid_path);
        bool Init(string guid_path);
        bool LoadAndInit(string guid_path);

        /// Returns the clip rect for the UTF-8 glyph in a particular point size, if it exists.
        /// If pointSize is <= 0, uses the last requested point size or the first imported glyph
        SDL_Rect GetGlyphClip(string utfChar, int pointSize = 0, int style = TTF_STYLE_NORMAL);

        /// Renders a single UTF-8 character
        void Render(Renderer& renderer, string utfChar, Vector2 position, int pointSize = 0, int style = TTF_STYLE_NORMAL, double angle = 0.0, Vector2 origin = {0.5f, 0.5f});
        void Render(Renderer& renderer, SDL_Rect dest, SDL_Rect clip, int pointSize = 0, int style = TTF_STYLE_NORMAL, double angle = 0.0, Vector2 origin = {0.5f, 0.5f});

    };

}

#endif // FONTPACK_H
