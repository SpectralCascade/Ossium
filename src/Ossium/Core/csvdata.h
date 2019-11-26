#ifndef CSVDATA_H
#define CSVDATA_H

#include <string>
#include <vector>

using namespace std;

namespace Ossium
{

    class OSSIUM_EDL CSV
    {
    public:
        /// Import a CSV file; set quoted to false if importing a csv file that doesn't wrap each cell in "" characters
        bool Import(string path, bool quoted = true);
        /// Export data to a CSV file
        void Export(string path, bool quoted = true);

        /// Returns a copy of the data contained in a single cell (zero based index)
        string GetCell(unsigned int row, unsigned int column);

        /// Returns a copy of the data in the specified row/column (zero based index)
        vector<string> GetRow(unsigned int index);
        vector<string> GetColumn(unsigned int index);

        /// Loaded data in a two dimensional dynamic array
        vector<vector<string>> data;

    };

}

#endif // CSVDATA_H
