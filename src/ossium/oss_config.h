#ifndef OSS_CONFIG_H
#define OSS_CONFIG_H

/// This struct contains configuration information for Ossium
struct OSS_Config
{
    bool fullscreen = 0;
    bool vsync = 0;
    char filtering = '1';
    unsigned int mastervolume = 100;
};

const bool OSS_CONFIG_DEFAULT_FULLSCREEN = false;
const bool OSS_CONFIG_DEFAULT_VSYNC = false;
const char OSS_CONFIG_DEFAULT_FILTERING = '1';
const unsigned int OSS_CONFIG_DEFAULT_MVOLUME = 100;

/// Loads configuration settings for Ossium
bool OSS_LoadConfig(OSS_Config* config);

/// Saves configuration settings for Ossium
bool OSS_SaveConfig(OSS_Config config);

#endif // OSS_CONFIG_H
