/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include <fstream>
#include <sstream>

#include "jsondata.h"
#include "funcutils.h"
#include "logging.h"

using namespace std;

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
                        if (!value.empty())
                        {
                            auto vlen = value.length();
                            // If it's a string, strip the double quotes.
                            if (vlen > 1 && value[0] == '"' && value[vlen - 1] == '"')
                            {
                                value.erase(vlen - 1);
                                value.erase(value.begin());
                            }
                            dataArray.push_back(value);
                        }
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
                            value = Strip(value);
                            value = Strip(value, '\n');
                            value = Strip(value, '\r');
                            value = Strip(value, '\n');
                            value = Strip(value);
                            if (!value.empty())
                            {
                                auto vlen = value.length();
                                // If it's a string, strip the double quotes.
                                if (vlen > 1 && value[0] == '"' && value[vlen - 1] == '"')
                                {
                                    value.erase(vlen - 1);
                                    value.erase(value.begin());
                                }
                                dataArray.push_back(value);
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
            Log.Warning("Attempted to convert JString value into array, but the value is not an array. Raw string:\n{0}", (*this));
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
                        value = Strip(value, '\r');
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
                            value = Strip(value, '\r');
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
            Log.Warning("Attempted to get element from JSON array, but this JString instance is not an array!");
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
        string toParse = Utilities::FileToString(path);
        if (toParse.empty())
        {
            Log.Warning("Failed to load JSON file '{0}'!", path);
            return false;
        }
        if (!Parse(toParse))
        {
            return false;
        }
        Log.Info("Imported JSON '{0}' successfully.", path);
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
                jsonStream << "\"" << itr.key() << "\"" << " : ";
                if (itr.value().IsString())
                {
                     jsonStream << "\"" << itr.value() << "\"";
                }
                else if (itr.value().IsJSON())
                {
                    /// Add indents
                    for (unsigned int i = 0; i < itr.value().length(); i++)
                    {
                        if (itr.value()[i] == '\n')
                        {
                            itr.value().insert(i + 1, string("    "));
                            i += 4;
                        }
                    }
                    jsonStream << itr.value();
                }
                else
                {
                    jsonStream << itr.value();
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

    void JSON::FromString(const string& str)
    {
        clear();
        Parse(str);
    }

    bool JSON::Parse(const string& json, unsigned int startIndex)
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
                        value = Strip(value, '\r');
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
            Log.Warning("Failed to parse JSON correctly due to bad formatting. Open: {0} | Is JSON: {1}", open ? "Yes" : "No", isJson ? "Yes" : "No");
            Log.Warning("JSON: {0}", json);
            return false;
        }
        return true;
    }

}
