#pragma once

#include "Interface/PluginInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class Win32PluginSystem
        : public ServiceBase<PluginSystemInterface>
    {
    public:
        Win32PluginSystem();
        ~Win32PluginSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const WString & _dynamicLibraryName ) override;

    protected:
        FactoryPtr m_factoryDynamicLibraries;
    };
}