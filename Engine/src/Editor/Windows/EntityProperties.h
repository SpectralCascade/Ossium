#ifndef ENTITYPROPERTIES_H
#define ENTITYPROPERTIES_H

#include "../../Core/jsondata.h"

#include "../Core/editorwindow.h"
#include "../Core/editorserializer.h"

namespace Ossium::Editor
{

    class EntityProperties : public EditorWindow, public Ossium::EditorSerializer
    {
    public:
        void OnInit();

        void OnGUI();

    protected:
        void Property(Ossium::JSON& data);

        void PropertyValue(Ossium::JString& data);

    };

}

#endif // ENTITYPROPERTIES_H
