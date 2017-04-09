#   pragma once

#   include "Interface/PluginInterface.h"

#   include "Core/ServiceBase.h"

namespace Menge
{
    class MockupPluginSystem
		: public ServiceBase<PluginSystemInterface>
    {
    public:
		MockupPluginSystem();
		~MockupPluginSystem();

    public:
		DynamicLibraryInterfacePtr loadDynamicLibrary( const WString & _dynamicLibraryName ) override;
	};
}