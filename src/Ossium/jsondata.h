#ifndef JSONDATA_H
#define JSONDATA_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

namespace Ossium
{

    /// Forward declaration
    class ArrayJSON;

    class JSON
    {
    public:
        ~JSON();

        /// Imports data from a JSON file. Note that calling this method clears all pre-existing data.
        bool Import(string path);
        /// Exports data to a JSON file.
        void Export(string path);

        /// Parses a string representing a JSON object and attempts to map the data.
        /// Note that calling this method clears all pre-existing data.
        bool Parse(string& json, unsigned int index = 0, JSON* jsonObj = nullptr);

        /// Clears all data in this JSON object.
        void Clear();

        /// Maps of key value pairs, where value types match the allowed JSON data types.
        /// Note that integers automatically become floats.
        unordered_map<string, string> data_strings;
        unordered_map<string, float> data_numbers;
        unordered_map<string, JSON*> data_objects;
        unordered_map<string, ArrayJSON*> data_arrays;
        unordered_map<string, bool> data_boolean;
        /// Empty values or values that are specified as null
        unordered_set<string> data_null;

    private:
        unsigned int ParseArray(string& json, unsigned int index, JSON* jsonObj = nullptr);

    };

    class ArrayJSON
    {
    public:
        ~ArrayJSON();

        void Clear();

        vector<string> data_strings;
        vector<float> data_numbers;
        vector<JSON*> data_objects;
        vector<ArrayJSON*> data_arrays;
        vector<bool> data_boolean;
    };

}

#endif // JSONDATA_H
