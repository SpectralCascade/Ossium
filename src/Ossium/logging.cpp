#include "logging.h"

namespace Ossium
{

    Logger& Logger::EngineLog()
    {
        engineLog = new Logger();
        return *engineLog;
    }

    Logger* Logger::engineLog = nullptr;

}
