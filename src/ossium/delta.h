#ifndef DELTA_H
#define DELTA_H

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
        void init();

    private:
        Uint32 previousTicks;

        float deltaTime;

    };

    namespace global
    {

        static Delta delta;

    }

}

#endif // DELTA_H
