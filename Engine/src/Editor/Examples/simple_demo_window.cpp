#include "simple_demo_window.h"

using namespace std;

namespace Ossium::Editor
{

    void SimpleDemoWindow::OnGUI()
    {
        Space(20);

        text = TextField(text);

        Space(20);

        BeginHorizontal();
        TextLabel("Bold: ");
        Tab();
        Tab();
        Tab();
        bold = Toggle(bold);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Italic: ");
        Tab();
        Tab();
        Tab();
        italic = Toggle(italic);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Underline: ");
        Tab();
        Tab();
        underline = Toggle(underline);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Strikethrough: ");
        Tab();
        Tab();
        strikethrough = Toggle(strikethrough);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Text Point Size: ");
        Tab();
        Tab();
        scale = Slider(scale, 8, 208, 200, 16, 16);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Text max width: ");
        Tab();
        width = Slider(width, 0.0f, 1024.0f, 200, 16, 16);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Text max height: ");
        Tab();
        height = Slider(height, 0.0f, 768.0f, 200, 16, 16);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Alignment Mode: ");
        Tab();
        alignment = (Typographic::TextAlignment)((int)Slider(alignment, 0.0f, 3, 200, 16, 16));
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Line wrap: ");
        Tab();
        Tab();
        Tab();
        lineWrap = Toggle(lineWrap);
        EndHorizontal();

        if (lineWrap)
        {
            BeginHorizontal();
            Tab();
            TextLabel("Word break: ");
            Tab();
            Tab();
            wordBreak = Toggle(wordBreak);
            EndHorizontal();
        }

        if (Button("Magic Button - Press me!"))
        {
            Log.Info("Boop!");
        }

    }

}
