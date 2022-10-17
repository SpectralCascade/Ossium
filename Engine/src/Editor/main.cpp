#ifdef __ANDROID__
#define OSSIUM_DEMO
#endif

#include "../Ossium.h"
#ifndef OSSIUM_DEMO
#include "Core/editorcontroller.h"

using namespace Ossium::Editor;
#endif
using namespace Ossium;
using namespace std;

int main(int argc, char* argv[])
{
    InitialiseOssium();

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

#ifdef OSSIUM_DEMO
    Vector2 origin = Vector2(640 / 2, 480 / 2);
    auto originMat = Matrix<2, 1>(
        {{origin.x, origin.y}}
    );

    // 3D cube

    // Upper back (or front?) left
    auto A = Matrix<3, 1>(
        {{-100, -100, -100}}
    );

    // Lower back left
    auto B = Matrix<3, 1>(
        {{-100, 100, -100}}
    );

    // Lower back right
    auto C = Matrix<3, 1>(
        {{100, 100, -100}}
    );

    // Upper back right
    auto D = Matrix<3, 1>(
        {{100, -100, -100}}
    );

    // Upper front right
    auto E = Matrix<3, 1>(
        {{100, -100, 100}}
    );

    // Lower front right
    auto F = Matrix<3, 1>(
        {{100, 100, 100}}
    );

    // Lower front left
    auto G = Matrix<3, 1>(
        {{-100, 100, 100}}
    );

    // Upper front left
    auto H = Matrix<3, 1>(
        {{-100, -100, 100}}
    );


    float scale = 1;

    // Projection matrix, from 3D to 2D.
    auto proj = Matrix<2, 3>({{scale, 0}, {0, 0}, {0, scale}});

    Window window("3D Experiment", 640, 480);
    Renderer renderer(&window, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    InputController input;

    bool quit = false;
    window.OnCloseButton += [&] (Window& caller) { quit = true; };
    
    SDL_Event e;
    float angle = 0;
    float angleSpeed = 1.2f;
    float tilt = 3.14159f / 8.0f;
    Timer timer;
    Clock clock;
    while (!quit)
    {
        timer.Start();
        while (SDL_PollEvent(&e) != 0)
        {
            window.HandleEvent(e);
            input.HandleEvent(e);
        }

        renderer.SetBackgroundColor(Colors::Black);
        SDL_RenderClear(renderer.GetRendererSDL());

        angle += clock.GetDeltaTime() * angleSpeed;

        // Rotate around the Z axis
        auto rotZ = Matrix<3, 3>({
            {cosf(angle), sinf(angle), 0},
            {-sinf(angle), cosf(angle), 0},
            {0, 0, 1}
        });

        // Rotate around the X axis
        auto rotX = Matrix<3, 3>({
            {1, 0, 0},
            {0, cosf(tilt), sinf(tilt)},
            {0, -sinf(tilt), cosf(tilt)},            
        });

        // Rotate around the X axis
        auto rotY = Matrix<3, 3>({
            {cosf(tilt), 0, -sinf(tilt)},
            {0, 1, 0},
            {sinf(tilt), 0, cosf(tilt)},
        });

        // Rotate about the X axis, then the Z axis, then project from 3D to 2D.
        auto projA = proj * (rotZ * (rotX * rotY * A));
        auto projB = proj * (rotZ * (rotX * rotY * B));
        auto projC = proj * (rotZ * (rotX * rotY * C));
        auto projD = proj * (rotZ * (rotX * rotY * D));
        auto projE = proj * (rotZ * (rotX * rotY * E));
        auto projF = proj * (rotZ * (rotX * rotY * F));
        auto projG = proj * (rotZ * (rotX * rotY * G));
        auto projH = proj * (rotZ * (rotX * rotY * H));

        // Now draw all 12 lines...
        // Back plane
        Line(
            Vector2(projA(0, 0), projA(0, 1)) + origin,
            Vector2(projB(0, 0), projB(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projB(0, 0), projB(0, 1)) + origin,
            Vector2(projC(0, 0), projC(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projC(0, 0), projC(0, 1)) + origin,
            Vector2(projD(0, 0), projD(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projD(0, 0), projD(0, 1)) + origin,
            Vector2(projA(0, 0), projA(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        // Sides
        Line(
            Vector2(projA(0, 0), projA(0, 1)) + origin,
            Vector2(projH(0, 0), projH(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projB(0, 0), projB(0, 1)) + origin,
            Vector2(projG(0, 0), projG(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projC(0, 0), projC(0, 1)) + origin,
            Vector2(projF(0, 0), projF(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projD(0, 0), projD(0, 1)) + origin,
            Vector2(projE(0, 0), projE(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        // Front plane
        Line(
            Vector2(projF(0, 0), projF(0, 1)) + origin,
            Vector2(projG(0, 0), projG(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projG(0, 0), projG(0, 1)) + origin,
            Vector2(projH(0, 0), projH(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projE(0, 0), projE(0, 1)) + origin,
            Vector2(projF(0, 0), projF(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);
        Line(
            Vector2(projH(0, 0), projH(0, 1)) + origin,
            Vector2(projE(0, 0), projE(0, 1)) + origin
        ).Draw(renderer, Colors::Blue);

        renderer.RenderPresent(true);
        SDL_RenderPresent(renderer.GetRendererSDL());

        clock.Update(((float)timer.GetTicks() / 1000.0f));
    }
#else
    ResourceController resources;
    EditorController* editor = new EditorController(&resources);

    while (editor->Update())
    {
        // Update until the editor stops running.
    }

    resources.FreeAll();

    delete editor;
    editor = nullptr;
#endif

    TerminateOssium();
    return 0;
}
