#include "services.h"

namespace Ossium
{

    ServicesProvider::~ServicesProvider()
    {
        delete[] services;
        services = nullptr;
    }

    void ServicesProvider::SetService()
    {
    }

}
