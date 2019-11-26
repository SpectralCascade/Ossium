#ifndef CONFIG_H
#define CONFIG_H

#include "schemamodel.h"
#include "stringconvert.h"

namespace Ossium
{

    /// General configuration information for Ossium
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
