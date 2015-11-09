#   include "PluginService.h"

#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginService, Menge::PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PluginService::PluginService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginService::~PluginService()
    {
		for( TVectorPlugins::iterator
            it = m_plugins.begin(),
            it_end = m_plugins.end();
        it != it_end;
        ++it )
        {
            PluginDesc & desc = *it;

			desc.plugin->finalize();
            desc.plugin->destroy();

			if( desc.dlib != nullptr )
			{
				desc.dlib->destroy();
			}
        }

        m_plugins.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	bool PluginService::_initialize()
	{
		//Empty

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginService::_finalize()
	{
		for( TVectorPlugins::iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			PluginDesc & desc = *it;

			desc.plugin->finalize();
			desc.plugin->destroy();

			if( desc.dlib != nullptr )
			{
				desc.dlib->destroy();
			}
		}

		m_plugins.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    PluginInterface * PluginService::loadPlugin( const WString & _dllName )
    {
		LOGGER_INFO( m_serviceProvider )("load Plugin %ls"
			, _dllName.c_str()
            );

		DynamicLibraryInterface * dlib = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->loadDynamicLibrary( _dllName );

		if( dlib == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [invalid load]"
				, _dllName.c_str()
				);

			return nullptr;
		}

		const Char * symbol = "dllCreatePlugin";

		TDynamicLibraryFunction function_dllCreatePlugin =
			dlib->getSymbol( symbol );

		if( function_dllCreatePlugin == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin symbol '%s'"
				, _dllName.c_str()
				, symbol
				);

			return nullptr;
		}

		TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

		PluginInterface * plugin;
		if( dllCreatePlugin( &plugin ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [invalid create]"
				, _dllName.c_str()
				);

			return nullptr;
		}

		if( plugin == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [plugin is NULL]"
				, _dllName.c_str()
				);

			return nullptr;
		}

		if( this->addPlugin( dlib, plugin ) == false )
		{
			return nullptr;
		}
		
        return plugin;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PluginService::addPlugin( DynamicLibraryInterface * _dlib, PluginInterface * _plugin )
	{
		if( _plugin == nullptr )
		{
			return false;
		}

		const Char * name = _plugin->getPluginName();

		_plugin->setServiceProvider( m_serviceProvider );

		if( _plugin->initialize() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin invalid initialize plugin '%s'"
				, name
				);

			return false;
		}
		
		if( this->hasPlugin( name ) == true )
		{
			return false;
		}

		PluginDesc desc;
		strcpy( desc.name, name );
		desc.dlib = _dlib;
		desc.plugin = _plugin;

		m_plugins.push_back( desc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginService::removePlugin( PluginInterface * _plugin )
	{
		if( _plugin == nullptr )
		{
			return false;
		}

		for( TVectorPlugins::iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			PluginDesc & desc = *it;

			if( desc.plugin == _plugin )
			{
				continue;
			}

			desc.plugin->finalize();
			desc.plugin->destroy();

			if( desc.dlib != nullptr )
			{
				desc.dlib->destroy();
			}

			m_plugins.erase( it );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginService::hasPlugin( const Char * _name ) const
	{
		for( TVectorPlugins::const_iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			const PluginDesc & desc = *it;

			if( desc.plugin == nullptr )
			{
				continue;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	PluginInterface * PluginService::getPlugin( const Char * _name ) const
	{
		for( TVectorPlugins::const_iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			const PluginDesc & desc = *it;

			if( desc.plugin == nullptr )
			{
				continue;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			PluginInterface * plugin = desc.plugin;

			return plugin;
		}

		return nullptr;
	}
}