#   pragma once

#   include "Interface/PluginInterface.h"

#   include "Core/ServiceBase.h"

namespace Mengine
{
    class MockupPluginSystem
		: public ServiceBase<PluginSystemInterface>
    {
    public:
		MockupPluginSystem();
		~MockupPluginSystem() override;

    public:
		DynamicLibraryInterfacePtr loadDynamicLibrary( const WString & _dynamicLibraryName ) override;
	};
}