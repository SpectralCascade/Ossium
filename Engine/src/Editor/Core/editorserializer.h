#ifndef EDITORSERIALIZER_H
#define EDITORSERIALIZER_H

#include "../../Core/stringconvert.h"
#include "../../Core/schematype.h"
#include "../../Core/jsondata.h"

namespace Ossium::Editor
{
    // Forward declaration
    class EditorWindow
    #ifndef OSSIUM_EDITOR
    {}
    #endif // OSSIUM_EDITOR
    ;
}

namespace Ossium
{

    class Vector2;

    enum EditorAttribute
    {
        ATTRIBUTE_NONE            = 0,        // Default value.
        ATTRIBUTE_HIDDEN          = 1,        // Hides the property (all types).
        ATTRIBUTE_FILEPATH        = 2,        // Treat property as a file path (strings only).
        ATTRIBUTE__RESERVED_03    = 4,        // Reserved for future use
        ATTRIBUTE__RESERVED_04    = 8,        // Reserved for future use
        ATTRIBUTE__RESERVED_05    = 16,       // Reserved for future use
        ATTRIBUTE__RESERVED_06    = 32,       // Reserved for future use
        ATTRIBUTE__RESERVED_07    = 64,       // Reserved for future use
        ATTRIBUTE__RESERVED_08    = 128,      // Reserved for future use
        ATTRIBUTE__RESERVED_09    = 256,      // Reserved for future use
        ATTRIBUTE__RESERVED_10    = 512,      // Reserved for future use
        ATTRIBUTE__RESERVED_11    = 1024,     // Reserved for future use
        ATTRIBUTE__RESERVED_12    = 2048,     // Reserved for future use
        ATTRIBUTE__RESERVED_13    = 4096,     // Reserved for future use
        ATTRIBUTE__RESERVED_14    = 8192,     // Reserved for future use
        ATTRIBUTE__RESERVED_15    = 16384,    // Reserved for future use
        ATTRIBUTE__CUSTOM_MIN     = 32768     // All custom properties should use this value and up (allows for 16 combinable attributes, including 0xFFFFFFFF).
    };

    class EditorSerializer
    {
    protected:
        // Initialise the serializer with an editor window it can use.
        void _InitEditorSerializer(Ossium::Editor::EditorWindow* window);

    public:
        // Sinkhole. Last method to be considered, caters for all types.
        template<typename ...Args>
        std::string SerializeProperty(const char* type, const char* name, int attribute, Args&&... args)
        {
            std::string data = Utilities::ToString(std::forward<Args>(args)...);
            return SerializeProperty(type, name, attribute, data);
        }

        //
        // TODO: Serialize enums as dropdowns!
        //

        /// Serialize a string value. This is the default implementation.
        std::string SerializeProperty(const char* type, const char* name, int attribute, std::string& property);

        /// Serialize a boolean. Defaults to a toggle.
        std::string SerializeProperty(const char* type, const char* name, int attribute, bool& property);

        /// Serialize a Vector2.
        std::string SerializeProperty(const char* type, const char* name, int attribute, Vector2& property);

        /// Serialize a schema derived type.
        template<typename T>
        typename std::enable_if<std::is_base_of<SchemaType, T>::value && !std::is_same<SchemaType, T>::value, std::string>::type
        SerializeProperty(const char* type, const char* name, int attribute, T& property)
        {
            // Serialize inline
            JSON data;
            property.SerialiseOut(data, this);
            return data.ToString();
        }
        
        // Resets the dirty flag that indicates whether any previous SerializeProperty() calls
        // modified a property value.
        void ClearDirtyFlag();

        // Returns true if any properties have been modified since the last ClearDirtyFlag() call.
        bool IsDirty();

    private:
        Ossium::Editor::EditorWindow* gui = nullptr;

        // Whether any property has been modified or not.
        bool dirty = false;

    };

}

#endif // EDITORSERIALIZER_H
