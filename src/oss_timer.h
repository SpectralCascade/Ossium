#ifndef OSS_TIMER_H
#define OSS_TIMER_H

#include <SDL2/SDL.h>

class OSS_Timer
{
public:
    /// Constructor initialises members
    OSS_Timer();

    /// Timer actions
    void start();
    void stop();
    void pause();
    void resume();

    /// Return the time since the timer has been started
    Uint32 getTicks();

    /// Return timer flags
    bool isStarted();
    bool isPaused();

private:
    /// Time according to SDL when timer is started
    Uint32 startTicks;

    /// Ticks stored while timer is paused
    Uint32 pausedTicks;

    /// Timer flags
    bool paused;
    bool started;

};

#endif // OSS_TIMER_H
