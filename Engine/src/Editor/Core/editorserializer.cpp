#include "editorserializer.h"
#ifdef OSSIUM_EDITOR
#include "editorwindow.h"
#endif

using namespace std;
using namespace Ossium::Editor;

namespace Ossium
{

    void EditorSerializer::_InitEditorSerializer(EditorWindow* window)
    {
        gui = window;
    }

    string EditorSerializer::SerializeProperty(string& property)
    {
#ifdef OSSIUM_EDITOR
        return gui->TextField(property);
#else
        return "";
#endif // OSSIUM_EDITOR
    }

}
