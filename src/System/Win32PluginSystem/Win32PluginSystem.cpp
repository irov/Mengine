#   include "Win32PluginSystem.h"

#   include "Win32DynamicLibrary.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Menge::Win32PluginSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	Win32PluginSystem::Win32PluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	Win32PluginSystem::~Win32PluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	DynamicLibraryInterface * Win32PluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
		LOGGER_WARNING( m_serviceProvider )("Win32PluginSystem::loadDynamicLibrary plugin '%ls'"
			, _dynamicLibraryName.c_str()
            );

		Win32DynamicLibrary * dynamicLibrary = new Win32DynamicLibrary( m_serviceProvider, _dynamicLibraryName );

		if( dynamicLibrary->load() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32PluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
				, _dynamicLibraryName.c_str()
				);

			dynamicLibrary->destroy();

			return nullptr;
		}
		
		return dynamicLibrary;
    }
}