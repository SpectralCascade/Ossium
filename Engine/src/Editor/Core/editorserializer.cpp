#ifdef OSSIUM_EDITOR
#include "editorwindow.h"
#endif
#include "editorserializer.h"

using namespace std;
using namespace Ossium::Editor;

namespace Ossium
{

    void EditorSerializer::_InitEditorSerializer(EditorWindow* window)
    {
        gui = window;
    }

    string EditorSerializer::SerializeProperty(const char* type, const char* name, int attribute, string& property)
    {
        Log.Info("Serializing string property...");
#ifdef OSSIUM_EDITOR
        if (gui->IsVisible() && !(attribute & ATTRIBUTE_HIDDEN))
        {
            gui->BeginHorizontal();
            gui->TextLabel(string(name), EditorStyle::StandardText);
            gui->Tab(100);
            string data = attribute & ATTRIBUTE_FILEPATH ? gui->FilePathField(property) : gui->TextField(property);
            gui->EndHorizontal();
            return data;
        }
#endif // OSSIUM_EDITOR
        return property;
    }

    string EditorSerializer::SerializeProperty(const char* type, const char* name, int attribute, bool& property)
    {
#ifdef OSSIUM_EDITOR
        if (gui->IsVisible() && !(attribute & ATTRIBUTE_HIDDEN))
        {
            gui->BeginHorizontal();
            gui->TextLabel(string(name), EditorStyle::StandardText);
            gui->Tab(100);
            bool data = gui->Toggle(property);
            gui->EndHorizontal();
            return data ? "1" : "0";
        }
#endif // OSSIUM_EDITOR
        return property ? "1" : "0";
    }

}
