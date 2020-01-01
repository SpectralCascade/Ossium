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
#include <string>
#include <vector>

#include "funcutils.h"
#include "csvdata.h"
#include "logging.h"

using namespace std;

namespace Ossium
{

    bool CSV::Import(string path, bool quoted)
    {
        data.clear();
        ifstream file(path.c_str());
        string line;
        unsigned int numColumns = 0;
        while (getline(file, line))
        {
            vector<string> row;
            data.push_back(row);
            int index = 0;
            string cell;
            for (int i = 0, counti = line.length(); i < counti; i++)
            {
                if (line[i] == ',')
                {
                    if (i == index)
                    {
                        index++;
                        data.back().push_back("");
                    }
                    else
                    {
                        cell = line.substr(index, i - index);
                        /// Remove quote marks from front and back of the string
                        if (quoted)
                        {
                            if (cell.length() > 2)
                            {
                                cell = cell.substr(1, cell.length() - 2);
                            }
                            else
                            {
                                /// empty string
                                cell = "";
                            }
                        }
                        data.back().push_back(cell);
                        index = i + 1;
                    }
                }
                if (i == counti - 1)
                {
                    if (i == index - 1)
                    {
                        data.back().push_back("");
                    }
                    else
                    {
                        cell = line.substr(index, i - (index - 1));
                        /// Remove quote marks from front and back of the string
                        if (quoted)
                        {
                            if (cell.length() > 2)
                            {
                                if (cell[0] == '"')
                                {
                                    cell = cell.substr(1, cell.length() - 2);
                                }
                            }
                            else
                            {
                                /// empty string
                                cell = "";
                            }
                        }
                        data.back().push_back(cell);
                    }
                }
            }
            unsigned int rowLength = data.back().empty() ? 0 : data.back().size();
            numColumns = numColumns < rowLength ? rowLength : numColumns;
        }
        if (data.empty() || data[0].empty())
        {
            Logger::EngineLog().Warning("Attempted to import CSV '{0}' but no data was loaded.", path);
            return false;
        }
        Logger::EngineLog().Info("Imported CSV '{0}' with {1} rows and {2} columns.", path, (int)data.size(), numColumns);
        return true;
    }

    void CSV::Export(string path, bool quoted)
    {
        ofstream file(path.c_str());
        for (auto i = data.begin(); i != data.end(); i++)
        {
            string line;
            for (unsigned int cell = 0, counti = (*i).empty() ? 0 : (*i).size(); cell < counti; cell++)
            {
                if (quoted && !IsNumber((*i)[cell]))
                {
                    line = line + '"';
                    line = line + (*i)[cell];
                    line = line + '"';
                }
                else
                {
                    line = line + (*i)[cell];
                }
                if (cell != (*i).size() - 1)
                {
                    line = line + ',';
                }
            }
            file << line << endl;
        }
    }

    string CSV::GetCell(unsigned int row, unsigned int column)
    {
        if (!data.empty() && row < data.size() && !data[row].empty() && column < data[row].size())
        {
            return data[row][column];
        }
        Logger::EngineLog().Warning("Could not get cell data from CSV, cell [row: {0}, column: {1}] is out of range!", row, column);
        return "";
    }

    vector<string> CSV::GetRow(unsigned int index)
    {
        if (!data.empty() && index < data.size())
        {
            return data[index];
        }
        Logger::EngineLog().Warning("Could not get row from CSV, row index [{0}] is out of range!", index);
        vector<string> none;
        return none;
    }

    vector<string> CSV::GetColumn(unsigned int index)
    {
        vector<string> column;
        if (!data.empty() && !data[0].empty() && index < data[0].size())
        {
            /// Iterate over each row and add the column index to the column
            for (auto i = data.begin(); i != data.end(); i++)
            {
                column.push_back((*i)[index]);
            }
        }
        else
        {
            Logger::EngineLog().Warning("Could not get column from CSV, column index [{0}] is out of range!", index);
        }
        return column;
    }

}
