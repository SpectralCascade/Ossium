#ifndef OSS_TEXTFILE_H
#define OSS_TEXTFILE_H

#include <string>

using namespace std;

/// Writes text to a file using SDL_RWops
void OSS_TextFileWrite(string filename, string text);

/// Reads a line of text from a file using SDL_RWops
string OSS_TextFileRead(string filename, unsigned int line = 0);

#endif // OSS_TEXTFILE_H
