#ifndef TIME_H
#define TIME_H

#include <SDL.h>

namespace Ossium
{
    /// A special clock that measures relative time passed
    class Clock
    {
    public:
        /// Takes the 'absolute' time at which the clock began, in milliseconds
        explicit Clock(Uint32 startTimeMS = 0);

        /// Updates the time passed on the clock according to the time scale factor
        void update(float deltaTime);

        /// Pause/Resume time
        void setPaused(bool pause);

        /// Return pause state
        bool isPaused();

        /// Stretch clock time by some scale factor
        void stretch(float scaleFactor);

        /// Returns the scale factor
        float getScaleFactor();

        /// Steps forward or backwards in time by a number of frames; defaults to 1/60th of a second per frame
        void stepFrames(int frames = 1, float framePeriod = 1.0f / 60.0f);

        /// Returns relative time passed in milliseconds
        Uint32 getTime();

        /// Returns the 'absolute' time that the clock began in milliseconds
        Uint32 getInitialTime();

        /// Returns the relative time difference between update() calls
        float getDeltaTime();

    private:
        /// Initial, 'absolute' time at which the clock began
        Uint32 initialTime;

        /// Relative time passed since clock began in milliseconds
        Uint32 time;

        /// Last value of time
        Uint32 previousTime;

        /// Scale factor to stretch/compress clock by
        float scale;

        /// Whether or not the clock is paused
        bool paused;

    };

    /// A simple timer, like a stop watch; can only be used for positive timing.
    /// As such, use of this class is limited to timeframes that aren't reversable
    class Timer
    {
    public:
        /// Takes a clock instance to provide timing ticks - otherwise defaults to absolute time
        Timer(Clock* refClock = NULL);
        Timer(const Timer& thisCopy);
        Timer operator=(const Timer& thisCopy);

        /// Timer actions; all parameters are in milliseconds and generally correspond to time passed
        /// since some arbitrary clock began - if no clock object is used, utilise SDL_GetTicks()
        void start();
        void stop();
        void pause();
        void resume();

        /// Return the relative time since the timer has been started
        Uint32 getTicks();

        /// Return timer flags
        bool isStarted();
        bool isPaused();

    private:
        /// Saves code duplication
        void copy(const Timer& thisCopy);

        /// Reference clock
        Clock* clock;

        /// Time at which the timer started, in milliseconds
        Uint32 startTicks;

        /// Ticks stored while timer is paused
        Uint32 pausedTicks;

        /// Timer flags
        bool paused;
        bool started;

    };

}

#endif // TIME_H
