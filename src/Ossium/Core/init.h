#ifndef INIT_H
#define INIT_H

namespace Ossium
{
    /// Initialisation error types
    enum InitError
    {
        INIT_ERROR_SDL = -9999,
        INIT_ERROR_IMG,
        INIT_ERROR_MIXER,
        INIT_ERROR_TTF
    };

    /// Initialises SDL and other engine subsystems; returns InitError value. No error if >= 0
    int InitialiseOssium();

    /// Terminates SDL and other engine subsystems
    void TerminateOssium();

}

#endif // INIT_H
