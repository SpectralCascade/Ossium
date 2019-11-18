#ifndef SERVICES_H
#define SERVICES_H

#include "logging.h"
#include "typefactory.h"

namespace Ossium
{

    namespace Internal
    {
        class ServiceBase
        {
        };
    }

    /// Non-static services locator.
    class ServicesProvider
    {
    public:

        /// Specify what services are provided in the constructor arguments.
        template<typename ...Args>
        ServicesProvider(Args&& ...args)
        {
            /// Instantiate services by type
            const Uint32 totalTypes = TypeSystem::TypeRegistry<Internal::ServiceBase>::GetTotalTypes();
            services = new Internal::ServiceBase*[totalTypes];
            for (unsigned int i = 0; i < totalTypes; i++)
            {
                services[i] = nullptr;
            }
            /// Now hook up the individual services
            SetService(forward<Args>(args)...);
        }

        virtual ~ServicesProvider();

        /// Attempts to return a pointer to a service matching the specified type.
        template<typename T>
        T* GetService()
        {
            Uint32 index = T::__services_entry.GetType();
            if (index < TypeSystem::TypeRegistry<Internal::ServiceBase>::GetTotalTypes())
            {
                if (services[index] == nullptr)
                {
                    Logger::EngineLog().Warning("Service of type [{0}] has not been set up!", index);
                }
                return reinterpret_cast<T*>(services[index]);
            }
            Logger::EngineLog().Warning("Failed to get service, invalid service type requested.");
            return nullptr;
        }

    protected:
        /// Services by type
        Internal::ServiceBase** services = nullptr;

    private:
        /// Base case
        void SetService();

        /// Recursive step through template parameter pack and set up the services.
        template<typename T, typename ...Args>
        void SetService(T service, Args&& ...args)
        {
            /// Recursive step first so services are set in reverse.
            /// This means if the same service type is provided more than once,
            /// the first instance of that type is used and the other instances are discarded.
            SetService(forward<Args>(args)...);
            services[remove_pointer<T>::type::__services_entry.GetType()] = service;
        }

    };

    /// Common engine services (e.g. Renderer, ResourceController) should inherit from this CRTP mix-in
    template<class Derived>
    class Service : public Internal::ServiceBase
    {
    public:
        friend class ServicesProvider;

    protected:
        static TypeSystem::TypeRegistry<Internal::ServiceBase> __services_entry;

    };

    template<class Derived>
    TypeSystem::TypeRegistry<Internal::ServiceBase> Service<Derived>::__services_entry;

}

#endif // SERVICES_H
