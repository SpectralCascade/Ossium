#include "simple_demo_window.h"

using namespace std;

namespace Ossium::Editor
{

    void SimpleDemoWindow::OnGUI()
    {
        TextLabel("Testing...");
        TextLabel("----------");
        TextLabel("I hope this works... haha ha hahahah hah ah aa ha ha hah ha h hah ha ha ahha ha hhahahahahaha ah ah ha hahahhaha ahah hahaha haha");
        TextLabel(":D");

        Space(50);

        BeginHorizontal();
        TextLabel("Output path: ");
        Tab();
        itext = TextField(itext);
        EndHorizontal();

        Space(50);

        BeginHorizontal();
        TextLabel("Checkbox 1: ");
        Tab();
        checkBox1 = Toggle(checkBox1);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Checkbox 2: ");
        Tab();
        checkBox2 = Toggle(checkBox2);
        EndHorizontal();

        Space(20);

        testSlider = Slider(testSlider, -1, 1);

        Space(50);

        Space(10);
        /*if (Button("Generate font pack", true, 40))
        {
            Logger::EngineLog().Info("Starting texture packing of font 'Orkney Regular'...");
            TexturePack fontPack = TexturePack();
            if (fontPack.ImportFont("../assets/Orkney Regular.ttf", 14, *renderer) && fontPack.ImportFont("../assets/Orkney Regular.ttf", 36, *renderer, "ł¶ŧ←↓→øþßðđŋħ«»¢“”µ·£¹²³€½¾"))
            {
                if (fontPack.PackImported(*renderer, SDL_PIXELFORMAT_ARGB8888, true, 256))
                {
                    fontPack.Save(*renderer, SDL_PIXELFORMAT_ARGB8888, "../cool_font_pack");
                }
            }
        }*/
        Space(10);
        if (Button("Save to JSON", 100, 20))
        {
            ofstream output(itext);
            if (output.is_open())
            {
                output <<  SimpleDemoSchema::ToString() << endl;
                Logger::EngineLog().Info("Saved data at '{0}'.", itext);
            }
            else
            {
                Logger::EngineLog().Warning("Failed to output JSON with given path '{0}'!", itext);
            }
        }

    }

}
