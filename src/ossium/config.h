#ifndef CONFIG_H
#define CONFIG_H

namespace Ossium
{
    const bool CONFIG_DEFAULT_FULLSCREEN = false;
    const bool CONFIG_DEFAULT_VSYNC = true;
    const float CONFIG_DEFAULT_FPS_CAP = 0;
    const char CONFIG_DEFAULT_FILTERING = '1';
    const unsigned int CONFIG_DEFAULT_MVOLUME = 100;

    /// This struct contains configuration information for Ossium
    struct Config
    {
        bool fullscreen = CONFIG_DEFAULT_FULLSCREEN;
        bool vsync = CONFIG_DEFAULT_VSYNC;
        float fpscap = CONFIG_DEFAULT_FPS_CAP;
        char filtering = CONFIG_DEFAULT_FILTERING;
        unsigned int mastervolume = CONFIG_DEFAULT_MVOLUME;
    };


    /// Loads configuration settings for Ossium
    bool LoadConfig(Config* config);

    /// Saves configuration settings for Ossium
    bool SaveConfig(Config config);

}

#endif // CONFIG_H
