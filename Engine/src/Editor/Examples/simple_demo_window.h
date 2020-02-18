/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
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
#ifndef SIMPLE_DEMO_WINDOW_H
#define SIMPLE_DEMO_WINDOW_H

#include "../Components/editorview.h"

namespace Ossium::Editor
{

    struct SimpleDemoSchema : public Schema<SimpleDemoSchema, 20>
    {
        DECLARE_BASE_SCHEMA(SimpleDemoSchema, 20);

        M(string, itext) = "testing.json";

        M(bool, checkBox1) = false;
        M(bool, checkBox2) = true;

        M(float, testSlider) = 0.75f;

    };

    class SimpleDemoWindow : public EditorWindow, public SimpleDemoSchema
    {
    public:
        CONSTRUCT_SCHEMA(EditorWindow, SimpleDemoSchema);

        /// TODO: remove me
        SimpleDemoWindow(Renderer* render, InputContext* inputContext, ResourceController* resourceController) :
            EditorWindow(render, inputContext, resourceController) { }

        /// Override of EditorWindow OnGUI method.
        virtual void OnGUI();

    };

}

#endif // SIMPLE_DEMO_WINDOW_H
