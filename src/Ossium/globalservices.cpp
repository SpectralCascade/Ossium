#include "globalservices.h"

namespace Ossium
{

    GlobalServices::GlobalServices()
    {
        Resources = new ResourceController();
    }

    ResourceController* GlobalServices::Resources = nullptr;

    Renderer* GlobalServices::MainRenderer = nullptr;

    AudioMixer GlobalServices::Mixer;

}
