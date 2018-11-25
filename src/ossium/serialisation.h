#ifndef SERIALISATION_H
#define SERIALISATION_H

#include <string>
#include <stack>
#include <SDL2/SDL.h>

#include "vector.h"

using namespace std;

namespace ossium
{

    const bool READ = 0;
    const bool WRITE = 1;

    // Forward declaration
    class Serialiser;

    // Any object that is serialised should inherit from this 'interface' class
    class SerialInterface
    {
    public:
        virtual void SerialRead(Serialiser* serial) = 0;
        virtual void SerialWrite(Serialiser* serial) = 0;
    };

    // Performs reading/writing of data, both in software and physically
    class Serialiser
    {
    public:
        Serialiser();
        ~Serialiser();

        // Starts a new sector for reading or writing; calls to this method can be nested
        void OpenSector(string name, string filePath, bool mode = READ);

        // Ends the sector that is currently being written,
        // similar in functionality to a closing bracket
        // Stores information about the sector's size in bytes
        void CloseSector(bool mode = READ);

        // Closes any open sectors and the current file stream
        void Close(bool mode = READ);

        //  File I/O read and write methods for basic data types
        template<typename T>
        void read(T &data)
        {
            if (file != NULL)
            {
                SDL_RWread(file, &data, sizeof(T), 1);
                index += sizeof(T);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Attempted to read from NULL file source! %d sectors open.",
                            openSectors.empty() ? 0 : (int)openSectors.size());
            }
        }

        template<typename T>
        void write(T &data)
        {
            if (file != NULL)
            {
                SDL_RWwrite(file, &data, sizeof(T), 1);
                index += sizeof(T);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Attempted to write to NULL file source! %d sectors open.",
                            openSectors.empty() ? 0 : (int)openSectors.size());
            }
        }

        // Read and write strings
        void readString(string &data);
        void writeString(string &data);

        // Read and write serial objects
        void readInterface(SerialInterface &obj)
        {
            obj.SerialRead(this);
        }
        void writeInterface(SerialInterface &obj)
        {
            obj.SerialWrite(this);
        }

        // Returns number of sectors read/written so far
        unsigned int getTotalSectors();

    private:
        // Prohibit copying
        Serialiser(Serialiser &thisCopy);
        Serialiser operator=(Serialiser &thisCopy);

        // File streaming reference
        SDL_RWops* file;

        // Current file path
        string path;

        // Current position in the file, as a byte offset relative to the sector name
        unsigned int index;

        // Total number of sectors written thus far
        unsigned int totalSectors;

        // Total number of data bytes written, not including sector data
        unsigned int totalDataBytes;

        // Total output filesize in bytes
        unsigned int fileSize;

        // Stack contains the size of sectors that are currently open when writing
        stack<unsigned int> openSectors;

    };

}

#endif // SERIALISATION_H
