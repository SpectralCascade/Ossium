#ifndef FONT_VIEWER_H
#define FONT_VIEWER_H

#include "../Core/editorwindow.h"

namespace Ossium::Editor
{

    class FontViewer : public EditorWindow
    {
    protected:
        void OnGUI();

        SDL_Color color = Colors::BLACK;

        std::string colorText = "000000";

        std::string inputFontPath = "assets/Orkney Regular.ttf";

        std::string currentFontPath = "assets/Orkney Regular.ttf";

        float scale = 1.0f;

    };

}

#endif // FONT_VIEWER_H
