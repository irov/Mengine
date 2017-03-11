#   include "Win32PluginSystem.h"

#   include "Win32DynamicLibrary.h"

#   include "Factory/FactoryPool.h"

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
	bool Win32PluginSystem::_initialize()
	{
		m_factoryDynamicLibraries = new FactoryPool<Win32DynamicLibrary, 8>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32PluginSystem::_finalize()
	{

	}
    //////////////////////////////////////////////////////////////////////////
	DynamicLibraryInterfacePtr Win32PluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
		LOGGER_WARNING( m_serviceProvider )("Win32PluginSystem::loadDynamicLibrary plugin '%ls'"
			, _dynamicLibraryName.c_str()
            );

		Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

		dynamicLibrary->setServiceProvider(m_serviceProvider);
		dynamicLibrary->setName(_dynamicLibraryName);

		if( dynamicLibrary->load() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32PluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
				, _dynamicLibraryName.c_str()
				);

			return nullptr;
		}
		
		return dynamicLibrary;
    }
}