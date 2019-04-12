#ifndef JSONDATA_H
#define JSONDATA_H

#include <string>
#include <vector>
#include <unordered_map>

#include "basics.h"

using namespace std;

namespace Ossium
{

    /// Forward declaration
    class JSON;

    /// String representation of data used in JSON
    class JString : public string
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
        JSON* ToJSON();

    };

    class JSON : public unordered_map<string, JString>
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
        string ToString();

    };

}

#endif // JSONDATA_H
