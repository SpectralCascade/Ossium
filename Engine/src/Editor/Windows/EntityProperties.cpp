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

        StyleClickable redStyle = EditorStyle::StandardButton;
        redStyle.hoverColor = Colors::Red;
        redStyle.normalColor = Color(230, 0, 0);
        redStyle.clickColor = Color(255, 80, 80);
        redStyle.normalStyleText.fg = Colors::White;
        redStyle.hoverStyleText.fg = Colors::White;
        redStyle.clickStyleText.fg = Colors::White;

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

            Tab(30);

            if (Button("Delete Entity", redStyle))
            {
                GetEditorLayout()->GetEditorController()->SelectEntity(nullptr);
                selected->Destroy(true);
                selected = nullptr;
                TriggerUpdate();
            }

        }
        else
        {
            TextLabel("<b>No entity selected.</b>");
        }

        EndHorizontal();

        Space(2);
        Line(Vector2(0, GetLayoutPosition().y), Vector2(viewport.w, GetLayoutPosition().y)).Draw(*renderer, Colors::Black);
        Space(2);

        if (selected != nullptr)
        {
            vector<BaseComponent*> modified;
            for (auto itr : selected->GetAllComponents())
            {
                for (BaseComponent* component : itr.second)
                {
                    BeginHorizontal();
                    TextLabel(Utilities::Format("<b>{0}</b>", GetComponentName((ComponentType)component->GetType())));
                    Tab(100);
                    if (Button("Remove Component", redStyle))
                    {
                        // Immediately destroy the component.
                        component->Destroy(true);
                        EndHorizontal();
                        TriggerUpdate();
                        continue;
                    }
                    EndHorizontal();

                    Space(4);

                    // Clear the dirty flag so we can check for changes
                    ClearDirtyFlag();

                    JSON data;
                    component->SerialiseOut(data, this);

                    //Log.Info("Serialized out to:\n" + data.ToString());

                    // Check if any properties changed during serialization.
                    if (IsDirty())
                    {
                        // Reload the component.
                        modified.push_back(component);
                        component->OnLoadStart();
                        component->SerialiseIn(data);
                        ClearDirtyFlag();
                    }

                    Space(2);
                    Line(Vector2(0, GetLayoutPosition().y), Vector2(viewport.w, GetLayoutPosition().y)).Draw(*renderer, Color(130, 130, 130));
                    Space(2);

                }
            }

            // Loading complete, update all modified components.
            for (auto component : modified)
            {
                component->OnLoadFinish();
                // Inform the component of the modification;
                // this is separate to OnLoadFinish() as that is also called
                // when the scene is first loaded.
                component->OnEditorPropertyChanged();
            }

            Space(4);

            Uint32 counti = GetTotalComponentTypes();
            vector<string> data;
            data.reserve(counti);
            for (Uint32 i = 0; i < counti; i++)
            {
                if (!IsAbstractComponent((ComponentType)i))
                {
                    data.push_back(GetComponentName((ComponentType)i));
                }
            }
            Dropdown<string>(
                [&] () { return Button("Add Component", true, 4, 4, true); },
                data,
                [selected, data] (unsigned int i) {
                    BaseComponent* component = selected->AddComponent(GetComponentType(data[i]));
                    if (component == nullptr)
                    {
                        Log.Warning("Failed to add component of type '{0}' to entity '{1}'!", GetComponentType(data[i]), selected->name);
                    }
                    else
                    {
                        // Pretend the component is loaded.
                        component->OnLoadStart();
                        component->OnLoadFinish();
                    }
                }
            );
        }

    }

    void EntityProperties::Property(JSON& data)
    {
        for (auto propertyItr = data.begin(); propertyItr != data.end(); propertyItr++)
        {
            BeginHorizontal();

            TextLabel(propertyItr.key() + ": ", EditorStyle::StandardText);
            Tab(100);

            PropertyValue(propertyItr.value());

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
