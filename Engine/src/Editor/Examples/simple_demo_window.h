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
