#ifndef DELTA_H
#define DELTA_H

#include "config.h"

namespace ossium
{

    class Delta
    {
    public:
        Delta();

        /// Get the change in time between calls to Update()
        float time();

        /// Updates values in the class each time it is called
        void update();

        /// Applies FPS capping configuration and initialises previousTicks to current ticks
        void init(Config& config);

        /// Resets previousTicks to current ticks
        void reset();

    private:
        Uint32 previousTicks;

        float deltaTime;

        float fpscap;

    };

    namespace global
    {

        static Delta delta;

    }

}

#endif // DELTA_H
