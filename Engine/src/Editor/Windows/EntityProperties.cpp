#include "EntityProperties.h"
#include "../Core/editorcontroller.h"
#include "../Core/editorstyle.h"
#include "../../Core/ecs.h"

using namespace Ossium;
using namespace std;

namespace Ossium::Editor
{

    void EntityProperties::OnInit()
    {
        _InitEditorSerializer(this);
    }

    void EntityProperties::OnGUI()
    {
        Entity* selected = GetEditorLayout()->GetEditorController()->GetSelectedEntity();

        BeginHorizontal();

        if (selected != nullptr)
        {
            TextLabel("Entity Name: ");
            Tab(30);
            selected->name = TextField(selected->name);

            Tab(76);

            Space(4);

            TextLabel("Active: ");
            Tab(30);
            bool result = Toggle(selected != nullptr ? selected->IsActiveLocally() : false);
            if (selected != nullptr)
            {
                selected->SetActive(result);
            }

        }
        else
        {
            TextLabel("<b>No entity selected.</b>");
        }

        EndHorizontal();

        Space(2);
        Line(Vector2(0, GetLayoutPosition().y), Vector2(viewport.w, GetLayoutPosition().y)).Draw(*renderer, Colors::BLACK);
        Space(2);

        if (selected != nullptr)
        {
            for (auto itr : selected->GetAllComponents())
            {
                if (!IsVisible())
                {
                    break;
                }
                for (BaseComponent* component : itr.second)
                {
                    if (!IsVisible())
                    {
                        break;
                    }

                    StyleClickable style = EditorStyle::StandardButton;
                    style.hoverColor = Colors::RED;
                    style.normalColor = Color(230, 0, 0);
                    style.clickColor = Color(255, 80, 80);
                    style.normalStyleText.fg = Colors::WHITE;
                    style.hoverStyleText.fg = Colors::WHITE;
                    style.clickStyleText.fg = Colors::WHITE;

                    BeginHorizontal();
                    TextLabel(Utilities::Format("<b>{0}</b>", GetComponentName((ComponentType)component->GetType())));
                    Tab(100);
                    if (Button("Remove Component", style))
                    {
                        // Immediately destroy the component.
                        component->Destroy(true);
                        EndHorizontal();
                        TriggerUpdate();
                        continue;
                    }
                    EndHorizontal();

                    Space(4);

                    JSON data;
                    component->SerialiseOut(data, this);

                    //Log.Info("Serialized out to:\n" + data.ToString());

                    component->OnLoadStart();
                    component->SerialiseIn(data);
                    component->OnLoadFinish();

                    Space(2);
                    Line(Vector2(0, GetLayoutPosition().y), Vector2(viewport.w, GetLayoutPosition().y)).Draw(*renderer, Color(130, 130, 130));
                    Space(2);

                }
            }

            Space(4);

            Uint32 counti = GetTotalComponentTypes();
            vector<string> data;
            data.reserve(counti);
            for (Uint32 i = 0; i < counti; i++)
            {
                data.push_back(GetComponentName((ComponentType)i));
            }
            Dropdown<string>(
                [&] () { return Button("Add Component", true, 4, 4, true); },
                data,
                [selected] (unsigned int i) {
                    BaseComponent* component = selected->AddComponent((ComponentType)i);
                    if (component == nullptr)
                    {
                        Log.Warning("Failed to add component of type '{0}' to entity '{1}'!", GetComponentName((ComponentType)i), selected->name);
                    }
                }
            );
        }

    }

    void EntityProperties::Property(JSON& data)
    {
        for (auto& property : data)
        {
            if (!IsVisible())
            {
                break;
            }

            BeginHorizontal();

            TextLabel(property.first + ": ", EditorStyle::StandardText);
            Tab(100);

            PropertyValue(property.second);

            EndHorizontal();
        }

    }

    void EntityProperties::PropertyValue(Ossium::JString& data)
    {
        // TODO: custom types (e.g. enums as dropdowns). Will likely require more SFINAE magic.
        if (data.IsInt())
        {
            JString result = TextField(data);
            // Only allow numeric characters.
            if (Utilities::IsInt(result))
            {
                data = result;
            }
        }
        else if (data.IsFloat())
        {
            JString result = TextField(data);
            // Only allow floats.
            if (Utilities::IsFloat(result))
            {
                data = result;
            }
        }
        else if (data.IsBool())
        {
            data = JString(Toggle(data.ToBool()) ? "true" : "false");
        }
        else if (data.IsArray())
        {
            auto dataArray = data.ToArray();

            BeginVertical();
            for (JString& element : dataArray)
            {
                // Recursive step
                PropertyValue(element);
            }
            EndVertical();
        }
        else if (data.IsJSON())
        {
            JSON* jdata = data.ToJSON();

            BeginVertical();

            // Extra recursive step
            Property(*jdata);
            data = jdata->ToString();

            EndVertical();

            delete jdata;
        }
        else
        {
            // Default to string.
            data = TextField(data);
        }
    }

}
