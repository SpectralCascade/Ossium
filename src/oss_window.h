#ifndef OSS_WINDOW_H
#define OSS_WINDOW_H

#include <string>
#include <SDL2/SDL.h>

/// Wrapper class for combined SDL_Window and SDL_Renderer
class OSS_Window
{
public:
    /// Appropriate constructor and destructor
    OSS_Window(const char* title = "Ossium Engine", int width = 640, int height = 480, bool fullscrn = false);
    ~OSS_Window();

    /// Handles window events
    void handle_events(SDL_Event &event);

    /// Get/Set specifiers
    SDL_Window* getWindow();
    void setFullScreen();
    void setWindowed();
    void setBorder(SDL_bool bordered);

    /// Return active flags
    bool isMinimized();
    bool isFullscreen();
    bool isFocus();

private:
    /// Individual window instance
    SDL_Window* window;

    /// Active flags
    bool minimized;
    bool fullscreen;
    bool focus;
    bool border;

};

#endif // OSS_WINDOW_H
