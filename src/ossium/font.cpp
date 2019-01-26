#include <string>
#include <map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "font.h"

using namespace std;

namespace ossium
{
    Font::Font()
    {
        font = NULL;
    }

    Font::~Font()
    {
        free();
    }

    void Font::free()
    {
        font = NULL;
        for (map<int, TTF_Font*>::iterator i = fontBank.begin(); i != fontBank.end(); i++)
        {
            TTF_Font* tempFont = i->second;
            if (tempFont != NULL)
            {
                TTF_CloseFont(tempFont);
                tempFont = NULL;
            }
        }
    }

    bool Font::load(string guid_path, int* pointSizes)
    {
        free();
        if (pointSizes == NULL)
        {
            /// Default (single) point size
            font = TTF_OpenFont(guid_path.c_str(), 24);
            fontBank[24] = font;
        }
        else if (pointSizes[0] <= 0)
        {
            /// Load the 20 default point sizes
            TTF_Font* tempFont = NULL;
            for (int i = 8; i <= 28; i < 14 ? i++ : i += 2)
            {
                tempFont = TTF_OpenFont(guid_path.c_str(), i);
                fontBank[i] = tempFont;
            }
            tempFont = TTF_OpenFont(guid_path.c_str(), 32);
            fontBank[32] = tempFont;
            tempFont = TTF_OpenFont(guid_path.c_str(), 36);
            fontBank[36] = tempFont;
            tempFont = TTF_OpenFont(guid_path.c_str(), 42);
            fontBank[42] = tempFont;
            tempFont = TTF_OpenFont(guid_path.c_str(), 48);
            fontBank[48] = tempFont;
            tempFont = TTF_OpenFont(guid_path.c_str(), 72);
            fontBank[72] = tempFont;
            /// Default selected point size of 24
            font = fontBank[24];
        }
        else
        {
            TTF_Font* tempFont = NULL;
            /// Pointsizes[0] specifies the amount of pointSizes[]
            for (int i = 1; i < pointSizes[0] + 1; i++)
            {
                tempFont = TTF_OpenFont(guid_path.c_str(), pointSizes[i]);
                if (tempFont == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open font '%s' with point size '%d'! TTF_Error: %s", guid_path.c_str(), pointSizes[i], TTF_GetError());
                    continue;
                }
                fontBank[pointSizes[i]] = tempFont;
            }
            /// Default to first provided pointsize
            font = fontBank[pointSizes[1]];
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

    TTF_Font* Font::getFont(int pointSize)
    {
        if (pointSize > 0)
        {
            TTF_Font* temp = NULL;
            temp = fontBank[pointSize];
            if (temp != NULL)
            {
                font = temp;
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Could not find font with point size '%d'. Defaulting to current selected font.", pointSize);
            }
        }
        return font;
    }

}
