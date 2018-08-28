#include <string>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>

#include "oss_config.h"

using namespace std;

bool OSS_LoadConfig(OSS_Config* config)
{
    bool success = true;

    /// Cannot take NULL config pointer!
    if (config == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OSS_LoadConfig can only take non-NULL argument!");
        return false;
    }

    /// Attempt to open file
    ifstream configFile("ossium.cfg");

    /// Check if file did not open
    if (!configFile.is_open())
    {
        /// Create a new config file if it doesn't exist or cannot be found
        SDL_Log("Could not open 'ossium.cfg'");
        SDL_Log("Creating new configuration file 'ossium.cfg'...");
        if (OSS_SaveConfig({OSS_CONFIG_DEFAULT_FULLSCREEN, OSS_CONFIG_DEFAULT_VSYNC, OSS_CONFIG_DEFAULT_FILTERING, OSS_CONFIG_DEFAULT_MVOLUME}))
        {
            configFile.open("ossium.cfg");
            if (!configFile.is_open())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not open configuration file 'ossium.cfg'!");
                success = false;
            }
        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create configuration file 'ossium.cfg'!");
            success = false;
        }
    }
    if (success)
    {
        /// Read config file
        string ignoredata;
        configFile >> ignoredata >> config->fullscreen >> ignoredata >> config->vsync >> ignoredata >> config->filtering >> ignoredata >> config->mastervolume;
        configFile.close();
    }
    return success;
}

bool OSS_SaveConfig(OSS_Config config)
{
    bool success = true;
    ofstream newConfigFile("ossium.cfg");
    /// Output settings to file
    if (newConfigFile.is_open())
    {
        newConfigFile << "Fullscreen: " << config.fullscreen << endl;
        newConfigFile << "V-Sync: " << config.vsync << endl;
        newConfigFile << "Anti-Aliasing: " << config.filtering << endl;
        newConfigFile << "Master Volume: " << config.mastervolume << endl;
        newConfigFile.close();
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not open configuration file!");
        success = false;
    }
    return success;
}
