#include <stack>
#include <string.h>
#include <SDL.h>

#include "coremaths.h"
#include "serialisation.h"

using namespace std;

namespace Ossium
{

    BinarySerialiser::BinarySerialiser()
    {
        file = NULL;
        path = "";
        index = 0;
        totalDataBytes = 0;
        totalSectors = 0;
        fileSize = 0;
    }

    BinarySerialiser::~BinarySerialiser()
    {
        // If the file or any sectors are still open, close them all
        Close();
    }

    void BinarySerialiser::Close(bool mode)
    {
        if (!openSectors.empty())
        {
            Logger::EngineLog().Info("Not all sectors were closed before closing the file. Closing {0} sectors now.", openSectors.size());
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
                Logger::EngineLog().Info("Closed file with final output size of {0} bytes and {1} sector(s). Wrote {2} bytes of data.", fileSize, totalSectors, totalDataBytes);
            }
            else
            {
                Logger::EngineLog().Info("Closed file '{0}'.", path);
            }
        }
        totalSectors = 0;
        fileSize = 0;
        totalDataBytes = 0;
        index = 0;
    }

    void BinarySerialiser::OpenSector(string name, string filePath, bool mode)
    {
        Logger::EngineLog().Verbose("Opening new sector '{0}' in file '{1}' with mode '{2}'.", name, filePath, mode == WRITE ? "binary write" : "binary read");
        if (file == NULL)
        {
            Logger::EngineLog().Info("Opening file '{0}' for {1}.", filePath, mode == WRITE ? "writing" : "reading");
            file = SDL_RWFromFile(filePath.c_str(), mode == WRITE ? "w+b" : "r+b");
            path = filePath;
            if (file == NULL)
            {
                Logger::EngineLog().Error("Failed to open file '{0}' for serialisation! SDL_Error: {1}", filePath, SDL_GetError());
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
                Logger::EngineLog().Error("Mismatched sector name! Attempted to read sector '{0}' but read '{1}'.", name, (const char*)sectorComparison);
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

    void BinarySerialiser::CloseSector(bool mode)
    {
        if (!openSectors.empty())
        {
            Logger::EngineLog().Verbose("Finished {0} a sector, {1} {2} bytes of data.", mode == READ ? "reading" : "writing", mode == READ ? "read" : "wrote", index);
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
            Logger::EngineLog().Warning("Attempted to close a sector but no sectors are open.");
        }
    }

    void BinarySerialiser::ReadString(string &data)
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
            Logger::EngineLog().Warning("Attempted to read from NULL file source! {0} sectors open.",
                        openSectors.empty() ? 0 : (int)openSectors.size());
        }
    }

    void BinarySerialiser::WriteString(string &data)
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
            Logger::EngineLog().Warning("Attempted to write to NULL file source! {0} sectors open.",
                        openSectors.empty() ? 0 : (int)openSectors.size());
        }
    }

    void BinarySerialiser::ReadInterface(SerialInterface &obj)
    {
        obj.SerialRead(*this);
    }

    void BinarySerialiser::WriteInterface(SerialInterface &obj)
    {
        obj.SerialWrite(*this);
    }

    unsigned int BinarySerialiser::GetTotalSectors()
    {
        return totalSectors;
    }

}
