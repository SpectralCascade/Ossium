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

        /// Initialises previousTicks to current ticks
        void init(Config& config);

        /// Resets the previous ticks variable to current time
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
