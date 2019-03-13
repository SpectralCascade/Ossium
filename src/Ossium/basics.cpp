#include <cstdarg>
#include <SDL.h>
#include <cstdio>
#include <string>

#include "basics.h"

using namespace std;

namespace Ossium
{

    inline namespace functions
    {

        float clamp(float n, float min, float max)
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

        int clamp(int n, int min, int max)
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

        int wrap(int n, int change, int min, int max)
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

        float mapRange(float value, float min, float max, float min_new, float max_new)
        {
            float fraction = clamp(value, min, max) / (max - min);
            return min_new + (fraction * (max_new - min_new));
        }

        float zeroToOne(float value)
        {
            if (value == 0)
            {
                return 1;
            }
            return value;
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

        string strip(string data)
        {
            for (int i = 0, counti = data.length(); i < counti; i++)
            {
                if (data[i] != ' ')
                {
                    data = data.substr(i, counti - i);
                    break;
                }
            }
            for (int i = data.length(); i > 0; i--)
            {
                if (data[i - 1] != ' ')
                {
                    data = data.substr(0, i);
                    break;
                }
            }
            return data;
        }

        string splitPair(string data, char delimiter, string outputOnError)
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

        string splitPairFirst(string data, char delimiter, string outputOnError)
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

        int ToInt(const string& data)
        {
            stringstream str;
            str.str("");
            str.str(data);
            int value = 0;
            if (!(str >> value))
            {
                #ifdef DEBUG
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to convert string '%s' to integer!", data.c_str());
                #endif // DEBUG
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
                #ifdef DEBUG
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to convert string '%s' to float!", data.c_str());
                #endif
            }
            return value;
        }

    }

}
