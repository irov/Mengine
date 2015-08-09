#   include "PluginService.h"

#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginService, Menge::PluginServiceInterface, Menge::PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PluginService::PluginService()
        : m_serviceProvider(nullptr)
        , m_dllCreatePluginName("dllCreatePlugin")
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginService::~PluginService()
    {
        for( TMapPlugins::iterator
            it = m_plugins.begin(),
            it_end = m_plugins.end();
        it != it_end;
        ++it )
        {
            const PluginDesc & desc = it->second;

			desc.plugin->finalize();
            desc.plugin->destroy();
            desc.dlib->destroy();
        }

        m_plugins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * PluginService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    PluginInterface * PluginService::loadPlugin( const WString & _name )
    {
		LOGGER_INFO( m_serviceProvider )("load Plugin %ls"
            , _name.c_str() 
            );

        TMapPlugins::iterator it_found = m_plugins.find( _name );

		if( it_found != m_plugins.end() )
		{
			LOGGER_WARNING( m_serviceProvider )("plugin %ls alredy exist"
				, _name.c_str()
				);

			return nullptr;
		}

		DynamicLibraryInterface * dlib = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->loadDynamicLibrary( _name );

		if( dlib == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [invalid load]"
				, _name.c_str()
				);

			return nullptr;
		}

		TDynamicLibraryFunction function_dllCreatePlugin =
			dlib->getSymbol( m_dllCreatePluginName );

		if( function_dllCreatePlugin == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [dllCreatePlugin]"
				, _name.c_str()
				);

			return nullptr;
		}

		TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

		PluginInterface * plugin;
		if( dllCreatePlugin( &plugin ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [invalid create]"
				, _name.c_str()
				);

			return nullptr;
		}

		if( plugin == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [plugin is NULL]"
				, _name.c_str()
				);

			return nullptr;
		}

		if( plugin->initialize( m_serviceProvider ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PluginService::loadPlugin can't load %ls plugin [invalid initialize]"
				, _name.c_str()
				);

			return nullptr;
		}

		PluginDesc desc;
		desc.dlib = dlib;
		desc.plugin = plugin;

		m_plugins.insert( std::make_pair( _name, desc ) );
		
        return plugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::unloadPlugin( const WString & _name )
    {
        TMapPlugins::iterator it_found = m_plugins.find( _name );

        if( it_found == m_plugins.end() )
        {
            return;
        }

        const PluginDesc & desc = it_found->second;

		desc.plugin->finalize();
        desc.plugin->destroy();
        desc.dlib->destroy();

        m_plugins.erase( it_found );
    }
}