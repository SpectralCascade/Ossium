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

        /// Load and post-load initialisation methods required for ResourceController
        /// Pass an integer array pointer; if NULL, a default point size is used.
        /// If value is <= 0, a default selection of point sizes are loaded,
        /// otherwise ptSize[0] is taken to be the total number of point sizes and
        /// the rest of the data specifies the actual point sizes
        bool Load(string guid_path, int* pointSizes = NULL);
        bool LoadAndInit(string guid_path, int* pointSizes = NULL);
        bool Init(string guid_path);

        /// Returns pointer to a font. If <= 0, get the current selected font. If the
        /// given pointsize is unavailable, by default the current font will be returned.
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

    };

}

#endif // FONT_H
