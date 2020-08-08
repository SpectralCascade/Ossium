#include "font_viewer.h"
#include "../Core/editorstyle.h"
#include <math.h>
#include <filesystem>

using namespace std;

namespace Ossium::Editor
{

    void FontViewer::OnGUI()
    {

        inputFontPath = FilePathField(inputFontPath);
        filesystem::path selectedPath = filesystem::path(inputFontPath);
        if (filesystem::exists(selectedPath))
        {
            currentFontPath = inputFontPath;
        }

        Space(5);

        BeginHorizontal();

        TextLabel("Color (hex):");

        Tab(20);

        colorText = TextField(colorText);

        if (IsValidColor(colorText))
        {
            color = Color(colorText);
        }

        EndHorizontal();

        Space(5);

        BeginHorizontal();

        TextLabel("Scale:");
        Tab(20);
        scale = Slider(scale, 0, 10);
        Space(5);
        TextLabel(Utilities::ToString(scale));

        EndHorizontal();

        Space(5);

        Font* font = resources->Get<Font>(currentFontPath, EditorStyle::StandardText.ptsize);
        if (font != nullptr)
        {
            font->Render(*renderer, Rect(0, GetLayoutPosition().y, scale * min((float)viewport.w, (float)font->GetAtlasSize()), scale * min((float)viewport.h, (float)font->GetAtlasSize())).SDL(), NULL, color);
        }
    }

}
