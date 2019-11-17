#include <SDL.h>
#include <SDL_ttf.h>

#include "font.h"
#include "logging.h"

using namespace std;

namespace Ossium
{
    Font::Font()
    {
        font = NULL;
    }

    Font::~Font()
    {
        Free();
    }

    void Font::Free()
    {
        font = NULL;
        if (!fontBank.empty())
        {
            if (TTF_WasInit() > 0)
            {
                for (auto i = fontBank.begin(); i != fontBank.end(); i++)
                {
                    TTF_Font* tempFont = i->second;
                    if (tempFont != NULL)
                    {
                        TTF_CloseFont(tempFont);
                        tempFont = NULL;
                    }
                }
            }
            fontBank.clear();
        }
    }

    bool Font::Load(string guid_path, int* pointSizes)
    {
        Free();
        if (pointSizes == NULL)
        {
            /// Default (single) point size
            font = TTF_OpenFont(guid_path.c_str(), 24);
            if (font != NULL)
            {
                fontBank[24] = font;
            }
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
            for (int i = 1; i < (pointSizes[0] + 1); i++)
            {
                tempFont = TTF_OpenFont(guid_path.c_str(), pointSizes[i]);
                if (tempFont == NULL)
                {
                    Logger::EngineLog().Error("Failed to open font '{0}' with point size '{1}'! TTF_Error: {2}", guid_path, pointSizes[i], TTF_GetError());
                    continue;
                }
                if (font == NULL)
                {
                    /// Default to first provided point size
                    font = tempFont;
                }
                fontBank[pointSizes[i]] = tempFont;
            }
        }
        if (font == NULL)
        {
            Logger::EngineLog().Error("Failed to open font '{0}'! TTF_Error: {1}", guid_path, TTF_GetError());
        }
        return font != NULL;
    }

    bool Font::Init(string guid_path)
    {
        return true;
    }

    TTF_Font* Font::GetFont(int pointSize)
    {
        if (pointSize > 0)
        {
            TTF_Font* temp = NULL;
            auto i = fontBank.find(pointSize);
            if (i != fontBank.end())
            {
                temp = i->second;
            }
            if (temp != NULL)
            {
                font = temp;
            }
            else
            {
                Logger::EngineLog().Warning("Could not find font with point size '{0}'. Defaulting to current selected font.", pointSize);
            }
        }
        return font;
    }

}
