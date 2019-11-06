#ifndef CONFIG_H
#define CONFIG_H

#include "ecs.h"

namespace Ossium
{

    /// This general configuration information for Ossium
    struct Config : public Schema<Config, 5>
    {
        DECLARE_BASE_SCHEMA(Config, 5);

        M(bool, fullscreen) = false;
        M(bool, vsync) = true;
        M(float, fpscap) = 0;
        M(char, filtering) = '1';
        M(unsigned int, mastervolume) = 100;

    };

}

#endif // CONFIG_H
