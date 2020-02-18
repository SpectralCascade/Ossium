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
