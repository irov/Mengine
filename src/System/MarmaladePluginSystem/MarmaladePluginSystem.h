#   pragma once

#   include "Interface/PluginInterface.h"

namespace Menge
{
    class MarmaladePluginSystem
		: public ServiceBase<PluginSystemInterface>
    {
    public:
		MarmaladePluginSystem();
		~MarmaladePluginSystem();

    public:
		DynamicLibraryInterface * loadDynamicLibrary( const WString & _dynamicLibraryName ) override;
	};
}