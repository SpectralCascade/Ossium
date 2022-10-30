#include "file.h"

namespace Ossium
{
    
    File::File(std::string path, std::string mode)
    {
        file = SDL_RWFromFile(path.c_str(), mode.c_str());
        if (file != NULL)
        {
            size = SDL_RWsize(file);
        }
        else
        {
            error = SDL_GetError();
        }
    }

    File::~File()
    {
        if (file != NULL)
        {
            SDL_RWclose(file);
        }
    }

    std::string File::ReadLine()
    {
        if (file != NULL)
        {
            std::string line;
            char buffer;
            // Read the data from the current index
            while (SDL_RWread(file, &buffer, sizeof(char), 1))
            {
                if (buffer == '\n')
                {
                    return line;
                }
                line.append(1, buffer);
            }

            if (SDL_RWtell(file) < size)
            {
                error = SDL_GetError();
            }
            else
            {
                return line;
            }
        }
        return "";
    }

    std::string File::ReadElement(char separator, bool ignoreNewlines)
    {
        std::string element = "";
        if (file != NULL && GetIndex() < size)
        {
            char buffer;
            while (SDL_RWread(file, &buffer, sizeof(char), 1))
            {
                if (buffer == separator || (buffer == '\n' && !ignoreNewlines))
                {
                    if (element.empty())
                    {
                        // Skip consecutive separator characters
                        continue;
                    }
                    return element;
                }
                element.append(1, buffer);
            }

            if (SDL_RWtell(file) < size)
            {
                error = SDL_GetError();
            }
            else
            {
                return element;
            }
        }
        return "";
    }

    bool File::GoTo(Sint64 index)
    {
        return index < size && file != NULL && SDL_RWseek(file, index, RW_SEEK_SET) >= 0;
    }

    Sint64 File::GetIndex()
    {
        return file != NULL ? SDL_RWtell(file) : 0;
    }

    std::string File::GetError()
    {
        return error;
    }

    bool File::HasError()
    {
        return !error.empty();
    }

    Sint64 File::Size()
    {
        return size;
    }

    std::string File::ToString()
    {
        if (file != NULL)
        {
            char* buffer = new char[size + 1];
            std::string data;
            // Read the data
            SDL_RWseek(file, 0, RW_SEEK_SET);
            Sint64 num_bytes = SDL_RWread(file, buffer, sizeof(char), size);
            if (num_bytes > 0)
            {
                // Null terminate and copy
                buffer[num_bytes] = '\0';
                data = std::string(buffer);
            }
            else
            {
                error = SDL_GetError();
            }

            // Clean up
            delete[] buffer;

            return data;
        }
        return "";
    }

}
