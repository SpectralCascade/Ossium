#ifndef EDITORSERIALIZER_H
#define EDITORSERIALIZER_H

#include "../../Core/stringconvert.h"

namespace Ossium::Editor
{
#ifdef OSSIUM_EDITOR
    // Forward declaration
    class EditorWindow;
#else
    typedef void EditorWindow;
#endif // OSSIUM_EDITOR
}

namespace Ossium
{

    class EditorSerializer
    {
    protected:
        // Initialise the serializer with an editor window it can use.
        void _InitEditorSerializer(Ossium::Editor::EditorWindow* window);

    public:
        // Generic unknown type.
        template<typename T>
        std::string SerializeProperty(T& property)
        {
            // Default to string.
            std::string data = Utilities::ToString(property);
            return SerializeProperty(data);
        }

        // Serialize a string value. This is the default implementation.
        std::string SerializeProperty(std::string& property);

    private:
        Ossium::Editor::EditorWindow* gui = nullptr;

    };

}

#endif // EDITORSERIALIZER_H
