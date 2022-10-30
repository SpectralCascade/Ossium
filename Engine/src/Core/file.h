#ifndef FILE_H
#define FILE_H

#include <SDL.h>

#include "funcutils.h"

namespace Ossium
{
    
    // Represents an open file stream.
    class File
    {
    public:
        // Open a file. Optionally specify file stream mode, internally this just makes a call to SDL_RWFromFile().
        // See https://wiki.libsdl.org/SDL_RWFromFile
        File(std::string path, std::string mode = "r");

        // Closes the file stream, if open.
        ~File();

        // Read a single line of the file from the current index.
        std::string ReadLine();

        // Read a single element in the file, distinguished by the separator.
        // Will skip over consecutive separator characters.
        // When this method returns an empty string, either an error has occurred or the end of the file has been reached.
        // Optionally ignore newlines, i.e. elements are distinguished solely by the separator and not newlines.
        std::string ReadElement(char separator = ' ', bool ignoreNewlines = false);

        // Navigate to a particular index in the file.
        bool GoTo(Sint64 index);

        // Return the current index in the file.
        Sint64 GetIndex();

        // Returns the last error message, if any.
        std::string GetError();

        // Returns true if some error occurred.
        bool HasError();

        // Get the entire file contents as a string.
        std::string ToString();

        // Returns the file size, in bytes.
        Sint64 Size();

    private:
        NOCOPY(File);

        // The last error message.
        std::string error = "";

        // Pointer to the file stream.
        SDL_RWops* file;

        // File size.
        Sint64 size = -1;

    };
    
}

#endif // FILE_H
