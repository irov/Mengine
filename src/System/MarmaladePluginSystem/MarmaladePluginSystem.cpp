#   include "MarmaladePluginSystem.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Menge::MarmaladePluginSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	MarmaladePluginSystem::MarmaladePluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	MarmaladePluginSystem::~MarmaladePluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	DynamicLibraryInterface * MarmaladePluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
		LOGGER_WARNING( m_serviceProvider )("MarmaladePluginSystem::loadDynamicLibrary plugin '%ls' not supported"
			, _dynamicLibraryName.c_str()
            );

		return nullptr;
    }
}