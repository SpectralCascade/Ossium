#include <fstream>
#include <sstream>

#include "jsondata.h"
#include "funcutils.h"
#include "logging.h"

namespace Ossium
{

    ///
    /// JString
    ///

    bool JString::IsInt()
    {
        return Utilities::IsInt((string)(*this));
    }
    bool JString::IsFloat()
    {
        return Utilities::IsFloat((string)(*this));
    }
    bool JString::IsNumber()
    {
        return Utilities::IsNumber((string)(*this));
    }
    bool JString::IsBool()
    {
        return Utilities::IsBool((string)(*this));
    }
    bool JString::IsArray()
    {
        return length() > 0 && (*this)[0] == '[' && (*this)[length() - 1] == ']';
    }
    bool JString::IsJSON()
    {
        return length() > 0 && (*this)[0] == '{' && (*this)[length() - 1] == '}';
    }
    bool JString::IsString()
    {
        return !IsNumber() && !IsBool() && !IsArray() && !IsJSON();
    }
    float JString::ToFloat()
    {
        return Utilities::ToFloat((string)(*this));
    }
    int JString::ToInt()
    {
        return Utilities::ToInt((string)(*this));
    }
    bool JString::ToBool()
    {
        return Utilities::ToBool((string)(*this));
    }
    vector<JString> JString::ToArray()
    {
        vector<JString> dataArray;
        if (IsArray())
        {
            int objCount = 0;
            int arrayCount = 0;
            bool parsingString = false;
            JString value;
            for (unsigned int i = 0, counti = length(); i < counti; i++)
            {
                if ((*this)[i] == '"' && (!parsingString || (i < 1 || (*this)[i - 1] != '\\')))
                {
                    parsingString = !parsingString;
                }
                if (!parsingString)
                {
                    if ((*this)[i] == ',' && arrayCount == 1 && objCount == 0)
                    {
                        value = Strip(value, '\n');
                        value = Strip(value);
                        dataArray.push_back(value);
                        value = (string)"";
                        continue;
                    }
                    else if ((*this)[i] == '[')
                    {
                        arrayCount++;
                        if (arrayCount <= 1)
                        {
                            continue;
                        }
                    }
                    else if ((*this)[i] == ']')
                    {
                        arrayCount--;
                        if (arrayCount < 1)
                        {
                            value = Strip(value, '\n');
                            value = Strip(value);
                            dataArray.push_back(value);
                            break;
                        }
                    }
                    else if ((*this)[i] == '{')
                    {
                        objCount++;
                    }
                    else if ((*this)[i] == '}')
                    {
                        objCount--;
                    }
                }
                value += (*this)[i];
            }
        }
        else
        {
            Logger::EngineLog().Warning("Attempted to convert JString value into array, but the value is not an array!");
        }
        return dataArray;
    }
    JString JString::ToElement(unsigned int arrayIndex)
    {
        JString value;
        if (IsArray())
        {
            unsigned int element_index = 0;
            int objCount = 0;
            int arrayCount = 1;
            bool parsingString = false;
            for (unsigned int i = 1, counti = length(); i < counti; i++)
            {
                if ((*this)[i] == '"' && (!parsingString || (i < 1 || (*this)[i - 1] != '\\')))
                {
                    parsingString = !parsingString;
                }
                if (!parsingString)
                {
                    if ((*this)[i] == ',' && arrayCount == 1 && objCount == 0)
                    {
                        value = Strip(value, '\n');
                        value = Strip(value);
                        value = Strip(value, '"');
                        if (element_index == arrayIndex)
                        {
                            return value;
                        }
                        element_index++;
                        value = string("");
                        continue;
                    }
                    else if ((*this)[i] == '[')
                    {
                        arrayCount++;
                    }
                    else if ((*this)[i] == ']')
                    {
                        arrayCount--;
                        if (arrayCount < 1)
                        {
                            value = Strip(value, '\n');
                            value = Strip(value);
                            value = Strip(value, '"');
                            if (element_index == arrayIndex)
                            {
                                return value;
                            }
                            break;
                        }
                    }
                    else if ((*this)[i] == '{')
                    {
                        objCount++;
                    }
                    else if ((*this)[i] == '}')
                    {
                        objCount--;
                    }
                }
                value += (*this)[i];
            }
        }
        else
        {
            Logger::EngineLog().Warning("Attempted to get element from JSON array, but this JString instance is not an array!");
        }
        /// Exceeded bounds special value
        return JString("\\!EB!\\");
    }
    JSON* JString::ToJSON()
    {
        return new JSON((string)(*this));
    }

    ///
    /// JSON
    ///

    JSON::JSON()
    {
    }

    JSON::JSON(string raw, unsigned int startIndex)
    {
        Parse(raw, startIndex);
    }

    bool JSON::Import(string path)
    {
        ifstream file(path.c_str());
        string toParse = Utilities::FileToString(file);
        if (toParse.empty())
        {
            Logger::EngineLog().Warning("Failed to load JSON file '{0}'!", path);
            return false;
        }
        file.close();
        if (!Parse(toParse))
        {
            return false;
        }
        Logger::EngineLog().Info("Imported JSON '{0}' successfully.", path);
        return true;
    }

    void JSON::Export(string path)
    {
        ofstream file(path.c_str());
        file << ToString();
        file.close();
    }

    string JSON::ToString(unsigned int indent_depth)
    {
        stringstream jsonStream;
        jsonStream.str("");
        for (unsigned int i = 0, counti = indent_depth; i < counti; i++)
        {
            jsonStream << "    ";
        }
        jsonStream << "{";
        if (!empty())
        {
            for (auto itr = begin(); itr != end();)
            {
                jsonStream << endl;
                for (unsigned int i = 0, counti = indent_depth + 1; i < counti; i++)
                {
                    jsonStream << "    ";
                }
                jsonStream << "\"" << itr->first << "\"" << " : ";
                if (itr->second.IsString())
                {
                     jsonStream << "\"" << itr->second << "\"";
                }
                else if (itr->second.IsJSON())
                {
                    /// Add indents
                    for (unsigned int i = 0; i < itr->second.length(); i++)
                    {
                        if (itr->second[i] == '\n')
                        {
                            itr->second.insert(i + 1, string("    "));
                            i += 4;
                        }
                    }
                    jsonStream << itr->second;
                }
                else
                {
                    jsonStream << itr->second;
                }
                if (++itr != end())
                {
                    jsonStream << ",";
                }
            }
        }
        jsonStream << endl;
        for (unsigned int i = 0, counti = indent_depth; i < counti; i++)
        {
            jsonStream << "    ";
        }
        jsonStream << "}";
        return jsonStream.str();
    }

    string JSON::ToString()
    {
        return ToString(0);
    }

    void JSON::FromString(string& str)
    {
        clear();
        Parse(str);
    }

    bool JSON::Parse(string& json, unsigned int startIndex)
    {
        bool open = false;
        bool keymode = false;
        bool parsingString = false;
        bool isJson = false;
        string key = "";
        string value = "";
        for (unsigned int i = startIndex, counti = json.length(); i < counti; i++)
        {
            if (!open)
            {
                if (json[i] == '{')
                {
                    open = true;
                    keymode = true;
                    isJson = true;
                }
            }
            else
            {
                if (keymode)
                {
                    if (parsingString)
                    {
                        if (json[i] == '"' && ((i > 0 && json[i - 1] != '\\') || (i > 1 && json[i - 2] == '\\')))
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
                    else if (json[i] == '}')
                    {
                        open = false;
                    }
                }
                else if (parsingString)
                {
                    if (json[i] == '"' && ((i > 0 && json[i - 1] != '\\') || (i > 1 && json[i - 2] == '\\')))
                    {
                        parsingString = false;
                    }
                    else
                    {
                        value += json[i];
                    }
                }
                else if (json[i] == '"')
                {
                    parsingString = true;
                }
                else if (json[i] == '{')
                {
                    /// Parse JSON into a single string
                    int objDepth = 0;
                    for (; i < counti; i++)
                    {
                        if (json[i] == '{')
                        {
                            objDepth++;
                        }
                        else if (json[i] == '}')
                        {
                            objDepth--;
                            if (objDepth < 1)
                            {
                                value += json[i];
                                break;
                            }
                        }
                        value += json[i];
                    }
                }
                else if (json[i] == '[')
                {
                    /// Parse array into a single string
                    int arrayDepth = 0;
                    for (; i < counti; i++)
                    {
                        if (json[i] == '[')
                        {
                            arrayDepth++;
                        }
                        else if (json[i] == ']')
                        {
                            arrayDepth--;
                            if (arrayDepth < 1)
                            {
                                value += json[i];
                                break;
                            }
                        }
                        value += json[i];
                    }
                }
                else if (json[i] != ' ')
                {
                    if (json[i] == ',' || json[i] == '}')
                    {
                        value = Strip(value, '\n');
                        value = Strip(value);
                        value = Strip(value, '"');
                        /// Add to data lookup
                        (*this)[key] = value;
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
        if (open || !isJson)
        {
            Logger::EngineLog().Warning("Failed to parse JSON correctly due to bad formatting.");
            return false;
        }
        return true;
    }

}
