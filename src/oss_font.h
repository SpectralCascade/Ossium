#ifndef OSS_FONT_H
#define OSS_FONT_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

class OSS_Font
{
public:
    OSS_Font();
    ~OSS_Font();

    /// Load and post-load initialisation methods required for OSS_ResourceController
    bool load(string guid_path, int* ptSize = NULL);
    bool init(string guid_path);

    /// Returns pointer to the font
    TTF_Font* getFont();

private:
    /// Pointer to actual font in memory
    TTF_Font* font;

};

#endif // OSS_FONT_H
