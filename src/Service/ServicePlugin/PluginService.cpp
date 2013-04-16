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
        : m_serviceProvider(NULL)
        , m_dllCreatePluginName("dllCreatePlugin")
    {
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
    PluginInterface * PluginService::loadPlugin( const String & _name )
    {
        LOGGER_INFO(m_serviceProvider)( "PluginService::loadPlugin get %s"
            , _name.c_str() 
            );

        TMapPlugins::iterator it_found = m_plugins.find( _name );

        if( it_found == m_plugins.end() )
        {
            LOGGER_INFO(m_serviceProvider)( "PluginService::loadPlugin load %s"
                , _name.c_str() 
                );

            WString unicode_name;
            Helper::utf8ToUnicode(m_serviceProvider, _name, unicode_name);

            DynamicLibraryInterface * dlib = WINDOWSLAYER_SERVICE(m_serviceProvider)
                ->loadDynamicLibrary( unicode_name );

            if( dlib == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "PluginService::loadPlugin can't load %s plugin [invalid load]"
                    , _name.c_str() 
                    );

                return NULL;
            }

            TDynamicLibraryFunction function_dllCreatePlugin =
                dlib->getSymbol( m_dllCreatePluginName );

            if( function_dllCreatePlugin == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "PluginService::loadPlugin can't load %s plugin [dllCreatePlugin]"
                    , _name.c_str() 
                    );

                return NULL;
            }

            TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

            PluginInterface * plugin;
            if( dllCreatePlugin( &plugin ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "PluginService::loadPlugin can't load %s plugin [invalid create]"
                    , _name.c_str() 
                    );

                return NULL;
            }

            if( plugin == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "PluginService::loadPlugin can't load %s plugin [plugin is NULL]"
                    , _name.c_str() 
                    );

                return NULL;
            }

            if( plugin->initialize( m_serviceProvider ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "PluginService::loadPlugin can't load %s plugin [invalid initialize]"
                    , _name.c_str() 
                    );

                return NULL;
            }
            
            PluginDesc desc;
            desc.dlib = dlib;
            desc.plugin = plugin;

            it_found = m_plugins.insert( std::make_pair(_name, desc) ).first;
        }

        const PluginDesc & desc = it_found->second;

        PluginInterface * plugin = desc.plugin;

        return plugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::unloadPlugin( const String & _name )
    {
        TMapPlugins::iterator it_found = m_plugins.find( _name );

        if( it_found == m_plugins.end() )
        {
            return;
        }

        const PluginDesc & desc = it_found->second;

        desc.plugin->destroy();
        desc.dlib->destroy();

        m_plugins.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::destroy()
    {
        for( TMapPlugins::iterator
            it = m_plugins.begin(),
            it_end = m_plugins.end();
        it != it_end;
        ++it )
        {
            const PluginDesc & desc = it->second;

            desc.plugin->destroy();
            desc.dlib->destroy();
        }

        m_plugins.clear();
    }
}