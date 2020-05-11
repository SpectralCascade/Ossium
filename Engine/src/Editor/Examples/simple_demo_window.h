#ifndef SIMPLE_DEMO_WINDOW_H
#define SIMPLE_DEMO_WINDOW_H

#include "../Components/editorview.h"
#include "../../Core/textlayout.h"

namespace Ossium::Editor
{

    struct SimpleDemoSchema : public Schema<SimpleDemoSchema, 20>
    {
        DECLARE_BASE_SCHEMA(SimpleDemoSchema, 20);

        M(float, scale) = 70.0f;

        M(bool, bold) = false;
        M(bool, italic) = false;
        M(bool, underline) = false;
        M(bool, strikethrough) = false;

        M(float, mipBias) = 0.5f;

        M(string, text) = "Hello!";

        M(float, width) = 580.0f;
        M(float, height) = 480.0f;

        M(Typographic::TextAlignment, alignment) = Typographic::TextAlignment::LEFT_ALIGNED;

        M(bool, lineWrap) = true;
        M(bool, wordBreak) = false;

    };

    class SimpleDemoWindow : public EditorWindow, public SimpleDemoSchema
    {
    public:
        CONSTRUCT_SCHEMA(EditorWindow, SimpleDemoSchema);

        /// Override of EditorWindow OnGUI method.
        virtual void OnGUI();

    };

}

#endif // SIMPLE_DEMO_WINDOW_H
