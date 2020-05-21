#include "mousecursor.h"

namespace Ossium
{

    MouseCursor::MouseCursor()
    {
        if (originalCursor == NULL)
        {
            originalCursor = SDL_GetCursor();
        }
    }

    MouseCursor::~MouseCursor()
    {
        Free();
        SDL_SetCursor(originalCursor);
    }

    void MouseCursor::Set(SDL_SystemCursor id)
    {
        SDL_Cursor* cursor = SDL_CreateSystemCursor(id);
        Instance.currentSystemCursor = id;
        if (cursor == NULL)
        {
            Logger::EngineLog().Warning("Failed to create system cursor. SDL_Error: {0}", SDL_GetError());
        }
        else
        {
            Set(cursor);
        }
    }

    void MouseCursor::Set(Image* image, int hotx, int hoty)
    {
        SDL_Cursor* cursor = SDL_CreateColorCursor(image->GetSurface(), hotx, hoty);
        if (cursor == NULL)
        {
            Logger::EngineLog().Warning("Failed to create custom cursor. SDL_Error: {0}", SDL_GetError());
        }
        else
        {
            Set(cursor);
        }
    }

    void MouseCursor::Set(SDL_Cursor* cursor)
    {
        Free();
        SDL_SetCursor(cursor);
        if (cursor == NULL)
        {
            Instance.currentSystemCursor = SDL_SYSTEM_CURSOR_ARROW;
        }
    }

    SDL_Cursor* MouseCursor::Get()
    {
        return SDL_GetCursor();
    }

    void MouseCursor::Clear()
    {
        Set((SDL_Cursor*)NULL);
    }

    void MouseCursor::Free()
    {
        SDL_Cursor* current = SDL_GetCursor();
        if (current != Instance.originalCursor)
        {
            if (current != NULL)
            {
                SDL_FreeCursor(current);
            }
        }
    }

    SDL_SystemCursor MouseCursor::GetCurrentSystemCursor()
    {
        return Instance.currentSystemCursor;
    }

}
