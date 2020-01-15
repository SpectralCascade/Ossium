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
extern "C"
{
    #include <SDL2/SDL_ttf.h>
}

#include "resourcecontroller.h"
#include "../Core/helpermacros.h"

using namespace std;

namespace Ossium
{

    class OSSIUM_EDL Font : public Resource
    {
    public:
        DECLARE_RESOURCE(Font);

        Font();
        ~Font();

        /// Frees all the fonts
        void Free();

        /// Loads a TrueType Font at different point sizes.
        /// TODO: Update to an SDL_TTF version that is > 2.0.15 and use DPI scaling instead of point size
        /// (TTF_SetFontSizeDPI() in the new API update).
        bool Load(string guid_path, vector<int> pointSizes = {8, 9, 10, 11, 12, 14, 18, 24, 30, 36, 48, 60, 72, 96});
        bool LoadAndInit(string guid_path, vector<int> pointSizes = {8, 9, 10, 11, 12, 14, 18, 24, 30, 36, 48, 60, 72, 96});

        bool Init(string guid_path);

        /// Returns the glyph clip rect for the given UTF-8 character.
        /** Internally, if the glyph is not in the GPU texture (at the given point size) already,
         * it first renders the glyph to a surface and then copies it to the texture.
         * If there is not enough space in the texture, glyphs that are least used in the texture are overwritten. */
        SDL_Rect GetGlyphClipRect(Renderer& renderer, string utf8char, int pointSize = 0, int style = TTF_STYLE_NORMAL);

        /// Renders the font atlas texture at a given point size. You should specify the clip rect using GetGlyphClipRect()
        /// to render an individual glyph.
        void Render(Renderer& renderer, SDL_Rect dest, SDL_Rect clip, int pointSize, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        /// Combines Render() and GetGlyphClipRect() to render a single glyph given a UTF-8 character.
        void RenderGlyph(Renderer& renderer, string utf8char, Vector2 position, int pointSize, int style, double angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /** Returns pointer to a font. If pointSize <= 0, get the current selected font. If the
         * given pointsize is unavailable, by default the current font will be returned.
         * Useful if you want to use SDL_ttf functions (e.g. to get glyph metrics). */
        TTF_Font* GetFont(int pointSize = 0);

    private:
        /// Copying is not permitted, as fonts are typically a large resource
        Font(const Font& thisCopy);
        Font operator=(const Font& thisCopy);

        /// Path to the font so it can be reloaded if necessary, e.g. when dealing with
        /// little memory but varying point sizes
        string path;

        /// Pointer to currently used font in memory
        TTF_Font* font;

        /// A bank of different point sizes for the same font
        map<int, TTF_Font*> fontBank;

        /// Map of point sizes to dynamic texture packs.
        map<int, DynamicTexturePack*> atlases;

    };

}

#endif // FONT_H
