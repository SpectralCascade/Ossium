#ifndef FONT_H
#define FONT_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

namespace ossium
{

    class Font
    {
    public:
        Font();
        ~Font();

        /// Load and post-load initialisation methods required for ResourceController
        bool load(string guid_path, int* ptSize = NULL);
        bool init(string guid_path);

        /// Returns pointer to the font
        TTF_Font* getFont();

    private:
        /// Pointer to actual font in memory
        TTF_Font* font;

    };

}

#endif // FONT_H
