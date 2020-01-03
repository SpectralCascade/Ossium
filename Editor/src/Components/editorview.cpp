/** COPYRIGHT NOTICE
 *
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include <Ossium.h>

#include "editorview.h"

namespace Ossium::Editor
{

    void EditorWindow::OnGUI()
    {
        TextLabel("Testing...");
        TextLabel("----------");
        //TextLabel("I hope this works... haha ha hahahah hah ah aa ha ha hah ha h hah ha ha ahha ha hhahahahahaha ah ah ha hahahhaha ahah hahaha haha");
        //TextLabel(":D");

        /*BeginHorizontal();
        TextField("Testing input...");
        Tab();
        TextField("000");
        Tab();
        TextField("100%");
        EndHorizontal();

        Space(50);

        BeginHorizontal();
        TextLabel("Checkbox 1: ");
        Tab();
        Toggle(false);
        EndHorizontal();

        BeginHorizontal();
        TextLabel("Checkbox 2: ");
        Tab();
        Toggle(true);
        EndHorizontal();

        Space(50);*/

        Space(10);
        if (Button("Do it", 40))
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
        }
        Space(10);
/*        if (Button("wow", 100, 20))
        {
            Logger::EngineLog().Info("Wowzas");
        }*/

    }

}
