/** COPYRIGHT NOTICE
 *  
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
