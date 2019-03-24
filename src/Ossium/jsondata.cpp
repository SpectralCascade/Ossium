#include <fstream>

#include "jsondata.h"
#include "basics.h"

namespace Ossium
{

    JSON::~JSON()
    {
        Clear();
    }

    void JSON::Clear()
    {
        data_strings.clear();
        data_numbers.clear();
        for (auto i : data_objects)
        {
            delete i.second;
        }
        data_objects.clear();
        data_boolean.clear();
        data_arrays.clear();
        data_null.clear();
    }

    bool JSON::Import(string path)
    {
        Clear();
        ifstream file(path.c_str());
        string toParse = ToString(file);
        if (!Parse(toParse))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to load JSON file '%s'!", path.c_str());
            return false;
        }
        SDL_Log("Imported JSON '%s' successfully.", path.c_str());
        return true;
    }

    void JSON::Export(string path)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "JSON export has not been implemented (yet).");
        SDL_assert(false);
    }

    bool JSON::Parse(string& json, unsigned int index, JSON* jsonObj)
    {
        if (jsonObj == nullptr)
        {
            jsonObj = this;
        }
        bool open = false;
        bool keymode = false;
        bool parsingString = false;
        string key = "";
        string value = "";
        for (unsigned int i = index, counti = json.length(); i < counti; i++)
        {
            if (!open)
            {
                if (json[i] == '{')
                {
                    open = true;
                    keymode = true;
                }
            }
            else
            {
                if (keymode)
                {
                    if (parsingString)
                    {
                        if (json[i] == '"' && (json[i - 1] != '\\' || json[i - 2] == '\\'))
                        {
                            parsingString = false;
                        }
                        else
                        {
                            key += json[i];
                        }
                    }
                    else if (json[i] == '"')
                    {
                        parsingString = true;
                    }
                    else if (json[i] == ':')
                    {
                        keymode = false;
                    }
                }
                else if (parsingString)
                {
                    if (json[i] == '"' && (json[i - 1] != '\\' || json[i - 2] == '\\'))
                    {
                        parsingString = false;
                    }
                    value += json[i];
                }
                else if (json[i] == '"')
                {
                    parsingString = true;
                    value += '"';
                }
                else if (json[i] == '{')
                {
                    JSON* recursiveObj = new JSON();
                    if (!Parse(json, i, recursiveObj))
                    {
                        return false;
                    }
                    jsonObj->data_objects[key] = recursiveObj;
                }
                else if (json[i] == '[')
                {
                    i = jsonObj->ParseArray(json, i, data_arrays[key]);
                    value = "";
                }
                else if (json[i] != ' ')
                {
                    if (json[i] == ',' || json[i] == '}')
                    {
                        value = strip(value, '\n');
                        /// Evaluate the value type and add it to the data
                        if (value.length() == 0 || value == "null")
                        {
                            jsonObj->data_null.insert(key);
                        }
                        if (value[0] == '"')
                        {
                            value = value.length() > 2 ? (value[value.length() - 1] == '"' ? value.substr(1, value.length() - 2) : value.substr(1, value.length() - 1)) : "";
                            jsonObj->data_strings[key] = value;
                        }
                        else if (value == "true")
                        {
                            jsonObj->data_boolean[key] = true;
                        }
                        else if (value == "false")
                        {
                            jsonObj->data_boolean[key] = false;
                        }
                        else if (IsFloat(value))
                        {
                            jsonObj->data_numbers[key] = ToFloat(value);
                        }
                        /// Reset temporary values
                        key = "";
                        value = "";
                        if (json[i] == '}')
                        {
                            open = false;
                            break;
                        }
                        keymode = true;
                    }
                    else
                    {
                        value += json[i];
                    }
                }
            }
        }
        return !open && !keymode;
    }

    unsigned int JSON::ParseArray(string& json, unsigned int index, vector<string>& data)
    {
        int arrayDepth = 0;
        int objDepth = 1;
        string value = "";
        for (unsigned int i = index, counti = json.length(); i < counti; i++)
        {
            if (json[i] == '[')
            {
                arrayDepth++;
                continue;
            }
            else if (json[i] == '{')
            {
                objDepth++;
            }
            else if (json[i] == '}')
            {
                objDepth--;
                if (objDepth < 1)
                {
                    return i;
                }
            }
            else if (arrayDepth >= objDepth && (json[i] == ',' || json[i] == ']'))
            {
                value = strip(value, '\n');
                data.push_back(value);
                value = "";
                if (json[i] == ']')
                {
                    arrayDepth--;
                    if (arrayDepth < 1)
                    {
                        return i;
                    }
                }
                continue;
            }
            value += json[i];
        }
        return json.length();
    }

}
