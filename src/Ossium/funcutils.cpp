#include <cstdarg>
#include <SDL.h>
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

        string FileToString(ifstream& fileStream)
        {
            if (!fileStream.is_open())
            {
                return "";
            }
            stringstream conversionStream;
            conversionStream << fileStream.rdbuf();
            return conversionStream.str();
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

        string SplitRight(string data, char delimiter, string outputOnError)
        {
            int index = data.find(delimiter);
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

        string SplitLeft(string data, char delimiter, string outputOnError)
        {
            int index = data.find(delimiter);
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
                    continue;
                }
                else if (data[i] == 'e' && isf && !exponent && i < counti - 1)
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
                Logger::EngineLog().Warning("Failed to convert string '{0}' to integer!", data);
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
                Logger::EngineLog().Warning("Failed to convert string '{0}' to float!", data);
            }
            return value;
        }

        bool ToBool(const string& data)
        {
            return data == "true" || data == "True" ? true : false;
        }

    }

}
