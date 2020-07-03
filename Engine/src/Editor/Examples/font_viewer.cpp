#include "font_viewer.h"
#include <math.h>

namespace Ossium::Editor
{

    void FontViewer::OnGUI()
    {

        BeginHorizontal();
        if (Button("Set Font"))
        {
            currentFontPath = inputFontPath;
        }

        Tab(20);

        inputFontPath = TextField(inputFontPath);
        EndHorizontal();

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

        Font* font = resources->Get<Font>(currentFontPath, NeuronStyles::NEURON_TEXT_NORMAL_STYLE.ptsize);
        if (font != nullptr)
        {
            font->Render(*renderer, Rect(0, GetLayoutPosition().y, scale * min((float)viewport.w, (float)font->GetAtlasSize()), scale * min((float)viewport.h, (float)font->GetAtlasSize())).SDL(), NULL, color);
        }
    }

}
