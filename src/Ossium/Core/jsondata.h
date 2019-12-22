/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#ifndef JSONDATA_H
#define JSONDATA_H

#include <string>
#include <vector>
#include <unordered_map>

#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    /// Forward declaration
    class JSON;

    /// String representation of data used in JSON
    class OSSIUM_EDL JString : public string
    {
    public:
        JString() : string()
        {
        }

        JString(string str) : string(str)
        {
        }

        bool IsInt();
        bool IsFloat();
        bool IsNumber();
        bool IsBool();
        bool IsArray();
        bool IsJSON();
        bool IsString();

        float ToFloat();
        int ToInt();
        bool ToBool();
        vector<JString> ToArray();
        JString ToElement(unsigned int arrayIndex);
        JSON* ToJSON();

    };

    class OSSIUM_EDL JSON : public unordered_map<string, JString>
    {
    public:
        JSON();
        JSON(string raw, unsigned int startIndex = 0);

        /// Imports data from a JSON file.
        /// Note that calling this method does NOT clear any pre-existing data.
        bool Import(string path);
        /// Exports data to a JSON file.
        void Export(string path);

        /// Parses a string representing a JSON object and attempts to map the data.
        /// Note that calling this method does NOT clear any pre-existing data.
        bool Parse(string& json, unsigned int startIndex = 0);

        /// Converts this JSON object into a string format representation.
        string ToString(unsigned int indent_depth);
        /// Ditto but defaults indent_depth to 0 and can be used for schema serialisation.
        string ToString();

        /// Same as Parse, but this one actually does clear the data first.
        void FromString(string& str);

    };

}

#endif // JSONDATA_H
