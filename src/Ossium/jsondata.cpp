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
        for (auto i : data_arrays)
        {
            delete i.second;
        }
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
        bool evaluateValue = false;
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
                    i = ParseArray(json, i + 1, jsonObj);
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
                            data_null.insert(key);
                        }
                        if (value[0] == '"')
                        {
                            value = value.length() > 2 ? (value[value.length() - 1] == '"' ? value.substr(1, value.length() - 2) : value.substr(1, value.length() - 1)) : "";
                            data_strings[key] = value;
                        }
                        else if (value == "true")
                        {
                            data_boolean[key] = true;
                        }
                        else if (value == "false")
                        {
                            data_boolean[key] = false;
                        }
                        else if (IsFloat(value))
                        {
                            data_numbers[key] = ToFloat(value);
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

    unsigned int JSON::ParseArray(string& json, unsigned int index, JSON* jsonObj)
    {
        if (jsonObj == nullptr)
        {
            jsonObj = this;
        }
        bool parsingString = false;
        bool isbool = false;
        bool isstring = false;
        bool isnum = false;
        bool isarray = false;
        bool isobj = false;
        for (unsigned int i = index, counti = json.length(); i < counti; i++)
        {
            /// TODO: parse arrays
            if (true)
            {
                return i;
            }
        }
        return json.length();
    }

    ArrayJSON::~ArrayJSON()
    {
        Clear();
    }

    void ArrayJSON::Clear()
    {
        for (auto i : data_arrays)
        {
            delete i;
        }
        for (auto i : data_objects)
        {
            delete i;
        }
        data_arrays.clear();
        data_objects.clear();
        data_boolean.clear();
        data_numbers.clear();
        data_strings.clear();
    }

}
