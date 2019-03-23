#include <fstream>
#include <string>
#include <vector>
#include <SDL.h>

#include "basics.h"
#include "csvdata.h"

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
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to import CSV '%s' but no data was loaded.", path.c_str());
            return false;
        }
        SDL_Log("Imported CSV '%s' with %d rows and %d columns.", path.c_str(), (int)data.size(), numColumns);
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
        SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not get cell data from CSV, cell [row: %d, column: %d] is out of range!", row, column);
        return "";
    }

    vector<string> CSV::GetRow(unsigned int index)
    {
        if (!data.empty() && index < data.size())
        {
            return data[index];
        }
        SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not get row from CSV, row index [%d] is out of range!", index);
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
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not get column from CSV, column index [%d] is out of range!", index);
        }
        return column;
    }

}
