#include "Win32PluginSystem.h"

#include "Win32DynamicLibrary.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Mengine::Win32PluginSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
	bool Win32PluginSystem::_initializeService()
	{
		m_factoryDynamicLibraries = new FactoryPool<Win32DynamicLibrary, 8>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32PluginSystem::_finalizeService()
	{
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
	DynamicLibraryInterfacePtr Win32PluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
		LOGGER_WARNING("Win32PluginSystem::loadDynamicLibrary plugin '%ls'"
			, _dynamicLibraryName.c_str()
            );

		Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

		dynamicLibrary->setName(_dynamicLibraryName);

		if( dynamicLibrary->load() == false )
		{
			LOGGER_ERROR("Win32PluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
				, _dynamicLibraryName.c_str()
				);

			return nullptr;
		}
		
		return dynamicLibrary;
    }
}