#ifndef OSS_TIMELINE_H
#define OSS_TIMELINE_H

#include <SDL2/SDL.h>

class OSS_TimeLine
{
public:
    /// Takes the 'absolute' time at which the timeline began, in milliseconds
    explicit OSS_TimeLine(Uint32 startTimeMS = 0);

    /// I don't think this method is necessary
    /*/// Return the difference in relative time between this timeline and another timeline, in seconds
    float difference(OSS_TimeLine &timeLine);*/

    /// Updates the time passed on the timeline according to the time scale factor
    void update(float deltaTime);

    /// Pause/Resume time
    void setPaused(bool pause);

    /// Return pause state
    bool isPaused();

    /// Stretch timeline by some scale factor
    void stretch(float scaleFactor);

    /// Returns the scale factor
    float getScaleFactor();

    /// Steps forward or backwards in time by a number of frames; defaults to 1/60th of a second per frame
    void stepFrames(int frames = 1, float framePeriod = 1.0f / 60.0f);

    /// Returns relative time passed in milliseconds
    Uint32 getTime();

    /// Returns the 'absolute' time that the timeline began in milliseconds
    Uint32 getInitialTime();

private:
    /// Initial, 'absolute' time at which the timeline began
    Uint32 initialTime;

    /// Relative time passed since timeline began in milliseconds
    Uint32 time;

    /// Scale factor to stretch/compress timeline by
    float scale;

    /// Whether or not the timeline is paused
    bool paused;

};

#endif // OSS_TIMELINE_H
