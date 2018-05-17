#   pragma once

#   include "Interface/PluginInterface.h"

#   include "Core/ServiceBase.h"
#   include "Factory/Factory.h"

namespace Mengine
{
    class SDLPluginSystem
        : public ServiceBase<PluginSystemInterface>
    {
    public:
        SDLPluginSystem();
        ~SDLPluginSystem();

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const WString & _dynamicLibraryName ) override;

    protected:
        FactoryPtr m_factoryDynamicLibraries;
    };
}
