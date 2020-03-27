#ifndef MOUSECURSOR_H
#define MOUSECURSOR_H

#include "lrucache.h"
#include "image.h"

namespace Ossium
{

    class MouseCursor : public Singleton<MouseCursor>
    {
    public:
        MouseCursor();
        ~MouseCursor();

        /// Sets the mouse cursor to a system cursor.
        static void Set(SDL_SystemCursor id);

        /// Sets the mouse cursor to a custom image. You must specify the hotspot (this is the position relative to the image which is used to get the mouse position).
        static void Set(Image* image, int hotx = 0, int hoty = 0);

        /// Sets the current SDL_Cursor directly.
        static void Set(SDL_Cursor* cursor);

        /// Returns the current SDL_Cursor.
        static SDL_Cursor* Get();

        /// Remove the mouse cursor entirely.
        static void Clear();

    private:
        /// Frees the current cursor.
        static void Free();

        /// The original mouse cursor.
        SDL_Cursor* originalCursor = NULL;

    };

}

#endif // MOUSECURSOR_H
