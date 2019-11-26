#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>
extern "C"
{
    #include <SDL_ttf.h>
}

#include "../Core/helpermacros.h"

using namespace std;

namespace Ossium
{

    class OSSIUM_EDL Font
    {
    public:
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
