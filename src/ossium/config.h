#ifndef CONFIG_H
#define CONFIG_H

namespace ossium
{
    /// This struct contains configuration information for Ossium
    struct Config
    {
        bool fullscreen = 0;
        bool vsync = 0;
        char filtering = '1';
        unsigned int mastervolume = 100;
    };

    const bool CONFIG_DEFAULT_FULLSCREEN = false;
    const bool CONFIG_DEFAULT_VSYNC = false;
    const char CONFIG_DEFAULT_FILTERING = '1';
    const unsigned int CONFIG_DEFAULT_MVOLUME = 100;

    /// Loads configuration settings for Ossium
    bool LoadConfig(Config* config);

    /// Saves configuration settings for Ossium
    bool SaveConfig(Config config);

}

#endif // CONFIG_H
