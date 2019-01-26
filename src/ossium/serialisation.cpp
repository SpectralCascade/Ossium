#include <stack>
#include <string.h>
#include <SDL.h>

#include "vector.h"
#include "serialisation.h"

using namespace std;

namespace ossium
{

    Serialiser::Serialiser()
    {
        file = NULL;
        path = "";
        index = 0;
        totalDataBytes = 0;
        totalSectors = 0;
        fileSize = 0;
    }

    Serialiser::~Serialiser()
    {
        // If the file or any sectors are still open, close them all
        Close();
    }

    void Serialiser::Close(bool mode)
    {
        if (!openSectors.empty())
        {
            SDL_Log("Not all sectors were closed before closing the file. Closing %d sectors now.", (int)openSectors.size());
            for (int i = 0, counti = openSectors.size(); i < counti; i++)
            {
                CloseSector(mode);
            }
        }
        if (file != NULL)
        {
            SDL_RWclose(file);
            file = NULL;
            if (mode == WRITE)
            {
                SDL_Log("Closed file with final output size of %d bytes and %d sector(s). Wrote %d bytes of data.", fileSize, totalSectors, totalDataBytes);
            }
            else
            {
                SDL_Log("Closed file '%s'.", path.c_str());
            }
        }
        totalSectors = 0;
        fileSize = 0;
        totalDataBytes = 0;
        index = 0;
    }

    void Serialiser::OpenSector(string name, string filePath, bool mode)
    {
        SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Opening new sector '%s' in file '%s' with mode '%s'.", name.c_str(), filePath.c_str(), mode == WRITE ? "binary write" : "binary read");
        if (file == NULL)
        {
            SDL_Log("Opening file '%s' for %s.", filePath.c_str(), mode == WRITE ? "writing" : "reading");
            file = SDL_RWFromFile(filePath.c_str(), mode == WRITE ? "w+b" : "r+b");
            path = filePath;
            if (file == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open file '%s' for serialisation! SDL_Error: %s", filePath.c_str(), SDL_GetError());
                return;
            }
        }
        if (mode == READ)
        {
            char* sectorComparison = new char[name.length() + 1];
            SDL_RWread(file, sectorComparison, sizeof(char), name.length());
            sectorComparison[name.length()] = '\0';
            if (*(const char*)sectorComparison != *name.c_str())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mismatched sector name! Attempted to read sector '%s' but read '%s'.", name.c_str(), (const char*)sectorComparison);
            }
            delete[] sectorComparison;
            sectorComparison = NULL;
        }
        else
        {
            SDL_RWwrite(file, name.c_str(), sizeof(char), name.length());
            fileSize += name.length();
        }
        if (!openSectors.empty())
        {
            openSectors.top() = index;
        }
        index = 0;
        openSectors.push(0);
        totalSectors++;
    }

    void Serialiser::CloseSector(bool mode)
    {
        if (!openSectors.empty())
        {
            SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Finished %s a sector, %s %d bytes of data.", mode == READ ? "reading" : "writing", mode == READ ? "read" : "wrote", index);
            totalDataBytes += index;
            fileSize += index;
            openSectors.pop();
            if (!openSectors.empty())
            {
                index = openSectors.top();
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Attempted to close a sector but no sectors are open.");
        }
    }

    void Serialiser::readString(string &data)
    {
        if (file != NULL)
        {
            int length = 0;
            SDL_RWread(file, &length, sizeof(int), 1);
            char* str = new char[length];
            SDL_RWread(file, str, sizeof(char), length);
            for (int i = 0; i < length - 1; i++)
            {
                data = data + str[i];
            }
            delete[] str;
            str = NULL;
            index += sizeof(int) + (length * sizeof(char));
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Attempted to read from NULL file source! %d sectors open.",
                        openSectors.empty() ? 0 : (int)openSectors.size());
        }
    }

    void Serialiser::writeString(string &data)
    {
        if (file != NULL)
        {
            // +1 for null terminator character
            int length = data.length() + 1;
            SDL_RWwrite(file, &length, sizeof(int), 1);
            SDL_RWwrite(file, (void*)data.c_str(), sizeof(char), length);
            index += sizeof(int) + (length * sizeof(char));
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Attempted to write to NULL file source! %d sectors open.",
                        openSectors.empty() ? 0 : (int)openSectors.size());
        }
    }

    void Serialiser::readInterface(SerialInterface &obj)
    {
        obj.SerialRead(this);
    }

    void Serialiser::writeInterface(SerialInterface &obj)
    {
        obj.SerialWrite(this);
    }

    unsigned int Serialiser::getTotalSectors()
    {
        return totalSectors;
    }

}
