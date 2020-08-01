#ifndef EDITORCONSTANTS_H
#define EDITORCONSTANTS_H

namespace Ossium::Editor
{

#ifdef _WIN32
    #define EDITOR_DEFAULT_DIRECTORY "C:\\"
    #define EDITOR_PATH_SEPARATOR "\\"
#else
    #define EDITOR_DEFAULT_DIRECTORY "/tmp"
    #define EDITOR_PATH_SEPARATOR "/"
#endif // _WIN32

}

#endif // EDITORCONSTANTS_H
