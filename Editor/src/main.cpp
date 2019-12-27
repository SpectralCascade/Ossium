#include <Ossium.h>

using namespace Ossium;

int main(int argc, char* argv[])
{

    Logger log;

    if (InitialiseOssium() >= 0)
    {
        /// Setup main window
        Window window("Ossium Editor");

        /// Setup services
        Renderer renderer(&window);
        ResourceController resources;
        InputController input;

        ServicesProvider services(&renderer, &resources, &input);

        EngineSystem engine(&services);

        while (engine.Update())
        {
            /// Do nothing
        }
    }
    else
    {
        log.Error("Failed to initialise Ossium.");
    }

    TerminateOssium();
}
