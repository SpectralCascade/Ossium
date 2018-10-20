#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "font.h"

namespace ossium
{
    Font::Font()
    {
        font = NULL;
    }

    Font::~Font()
    {
        if (font != NULL)
        {
            TTF_CloseFont(font);
            font = NULL;
        }
    }

    bool Font::load(string guid_path, int* ptSize)
    {
        if (font != NULL)
        {
            TTF_CloseFont(font);
            font = NULL;
        }
        if (ptSize == NULL)
        {
            font = TTF_OpenFont(guid_path.c_str(), 24);
        }
        else
        {
            font = TTF_OpenFont(guid_path.c_str(), *ptSize);
        }
        if (font == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open font '%s'! TTF_Error: %s", guid_path.c_str(), TTF_GetError());
        }
        return font != NULL;
    }

    bool Font::init(string guid_path)
    {
        return true;
    }

    TTF_Font* Font::getFont()
    {
        return font;
    }

}