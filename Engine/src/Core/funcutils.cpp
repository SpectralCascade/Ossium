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
#include <cstdarg>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>

#include "funcutils.h"
#include "logging.h"

using namespace std;

namespace Ossium
{

    inline namespace Utilities
    {

        float Clamp(float n, float min, float max)
        {
            if (n < min)
            {
                return min;
            }
            else if (n > max)
            {
                return max;
            }
            return n;
        }

        int Clamp(int n, int min, int max)
        {
            if (n < min)
            {
                return min;
            }
            else if (n > max)
            {
                return max;
            }
            return n;
        }

        int Wrap(int n, int change, int min, int max)
        {
            int wrapped = n + change;
            if (change > max - min)
            {
                wrapped = n + (change % ((max - min) + 1));
            }
            else if (change < min - max)
            {
                wrapped = n + (change % ((max - min) + 1));
            }
            if (wrapped > max)
            {
                wrapped = min + (wrapped - (max + 1));
            }
            else if (wrapped < min)
            {
                wrapped = max - ((min - 1) - wrapped);
            }
            return wrapped;
        }

        float MapRange(float value, float min, float max, float min_new, float max_new)
        {
            float fraction = Clamp(value, min, max) / (max - min);
            return min_new + (fraction * (max_new - min_new));
        }

        string ToString(float n)
        {
            stringstream strStream;
            strStream.str("");
            strStream << n;
            return strStream.str();
        }

        string ToString(int n)
        {
            stringstream strStream;
            strStream.str("");
            strStream << n;
            return strStream.str();
        }

        string FileToString(string path)
        {
            string data;
            // Open the file
            SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
            Sint64 fsize = -1;
            if (file != NULL && (fsize = SDL_RWsize(file)) >= 0)
            {
                char* buffer = new char[fsize + 1];
                // Read the data
                Sint64 num_bytes = SDL_RWread(file, buffer, sizeof(char), fsize);
                if (num_bytes > 0)
                {
                    // Null terminate and copy
                    buffer[num_bytes] = '\0';
                    data = string(buffer);
                }

                // Clean up
                delete[] buffer;
                SDL_RWclose(file);
            }
            else
            {
                // File could not be opened
                // TODO: return in error message object
                Log.Warning("Failed to open file at path \"{0}\". {1}", path, SDL_GetError());
            }
            return data;
        }

        string Strip(string data, char optionalChar)
        {
            for (int i = 0, counti = data.length(); i < counti; i++)
            {
                if (data[i] != optionalChar)
                {
                    data = data.substr(i, counti - i);
                    break;
                }
            }
            for (int i = data.length(); i > 0; i--)
            {
                if (data[i - 1] != optionalChar)
                {
                    data = data.substr(0, i);
                    break;
                }
            }
            return data;
        }

        string StripFilename(string path)
        {
            for (int i = path.size() - 1; i >= 0; i--)
            {
                if (path[i] == '/' || path[i] == '\\')
                {
                    path = path.substr(0, i + 1);
                    break;
                }
            }
            return path;
        }

        string ExtractFilename(string path)
        {
            for (int i = path.size() - 1; i >= 0; i--)
            {
                if (path[i] == '/' || path[i] == '\\')
                {
                    int index = i + 1;
                    if (index < (int)path.size())
                    {
                        path = path.substr(index, path.size() - index);
                    }
                    break;
                }
            }
            return path;
        }

        string SanitiseFilename(string name)
        {
            name = Strip(name);
            string sanitised;
            for (unsigned int i = 0, counti = name.size(); i < counti; i++)
            {
                char c = name[i];
                if (c == '<' || c == '>' || c == '"' || c == '|' || c == ':' || c == '/' || c == '\\' || c == '?' || c == '*' || c == '%')
                {
                    continue;
                }
                sanitised += c;
            }
            return sanitised;
        }

        string SplitRight(string data, char delimiter, string outputOnError, int dir)
        {
            int index = dir >= 0 ? data.find(delimiter) : data.rfind(delimiter);
            if (data[index] != delimiter)
            {
                if (outputOnError == "%s")
                {
                    return data;
                }
                return outputOnError;
            }
            return data.substr(index + 1);
        }

        string SplitLeft(string data, char delimiter, string outputOnError, int dir)
        {
            int index = dir >= 0 ? data.find(delimiter) : data.rfind(delimiter);
            if (data[index] != delimiter)
            {
                if (outputOnError == "%s")
                {
                    return data;
                }
                return outputOnError;
            }
            return data.substr(0, index);
        }

        vector<string> Split(string data, char delimiter)
        {
            vector<string> out;
            unsigned int startIndex = 0;
            unsigned int counti = data.length();
            for (unsigned int i = 0; i < counti; i++)
            {
                if (data[i] == delimiter && (startIndex != i - 1 || (i == 1 && data[0] != delimiter)))
                {
                    out.push_back(data.substr(startIndex, i - startIndex));
                    startIndex = i + 1;
                }
            }
            if (startIndex < counti)
            {
                out.push_back(data.substr(startIndex, counti - startIndex));
            }
            return out;
        }

        bool IsInt(const string& data)
        {
            bool isi = false;
            for (int i = 0, counti = data.length(); i < counti; i++)
            {
                if (data[i] > 47 && data[i] < 58)
                {
                    isi = true;
                }
                else if (i == 0 && data[i] == '-')
                {
                    continue;
                }
                else
                {
                    return false;
                }
            }
            return isi;
        }

        bool IsFloat(const string& data)
        {
            bool isf = false;
            bool singlepoint = false;
            bool exponent = false;
            for (int i = 0, counti = data.length(); i < counti; i++)
            {
                if (data[i] > 47 && data[i] < 58)
                {
                    isf = true;
                }
                else if (data[i] == '.' && !singlepoint)
                {
                    singlepoint = true;
                }
                else if (data[i] == '-')
                {
                    // TODO: Be more strict here maybe?
                    continue;
                }
                else if (data[i] == 'e' && isf && !exponent)
                {
                    exponent = true;
                }
                else
                {
                    return false;
                }
            }
            return isf;
        }

        bool IsNumber(const string& data)
        {
            return IsInt(data) || IsFloat(data);
        }

        bool IsBool(const string& data)
        {
            return data == "true" || data == "false" || data == "True" || data == "False";
        }

        bool IsString(const string& data)
        {
            return data.length() > 0 && data[0] == '"' && data[data.length() - 1] == '"';
        }

        int ToInt(const string& data)
        {
            stringstream str;
            str.str("");
            str.str(data);
            int value = 0;
            if (!(str >> value))
            {
                //Log.Warning("Failed to convert string '{0}' to integer!", data);
            }
            return value;
        }

        Uint32 ToUint32FromHex(const string& data)
        {
            Uint32 value = 0;
            for (auto c : data)
            {
                value = value << 4;
                if (c >= '0' && c <= '9')
                {
                    value |= (Uint32)(c - '0');
                }
                else if (c >= 'A' && c <= 'F')
                {
                    value |= (Uint32)((c + 10) - 'A');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    value |= (Uint32)((c + 10) - 'a');
                }
                else
                {
                    //Log.Warning("Failed to convert string '{0}' to integer from hexadecimal!", data);
                    break;
                }
            }
            return value;
        }

        float ToFloat(const string& data)
        {
            stringstream str;
            str.str("");
            str.str(data);
            float value = 0;
            if (!(str >> value))
            {
                //Log.Warning("Failed to convert string '{0}' to float!", data);
            }
            return value;
        }

        bool ToBool(const string& data)
        {
            return data == "true" || data == "True" ? true : false;
        }

    }

}
