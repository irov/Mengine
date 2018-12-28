#include "PluginService.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/Logger.h"

#include <string.h>

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginService, Mengine::PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginService::PluginService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginService::~PluginService()
    {
        for( PluginDesc & desc : m_plugins )
        {
            desc.plugin->finalizePlugin();
        }

        m_plugins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::_finalizeService()
    {
        VectorPlugins reverse_plugins = m_plugins;
        std::reverse( reverse_plugins.begin(), reverse_plugins.end() );

        m_plugins.clear();

        for( PluginDesc & desc : reverse_plugins )
        {
            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::loadPlugin( const Char * _dllName )
    {
        LOGGER_WARNING( "load Plugin '%s'"
            , _dllName
        );

        WChar unicode_dllName[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _dllName, unicode_dllName, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        DynamicLibraryInterfacePtr dlib = PLUGIN_SYSTEM()
            ->loadDynamicLibrary( unicode_dllName );

        if( dlib == nullptr )
        {
            LOGGER_ERROR( "PluginService::loadPlugin can't load '%s' plugin [invalid load]"
                , _dllName
            );

            return false;
        }

        const Char * symbol = "dllCreatePlugin";

        TDynamicLibraryFunction function_dllCreatePlugin =
            dlib->getSymbol( symbol );

        if( function_dllCreatePlugin == nullptr )
        {
            LOGGER_ERROR( "PluginService::loadPlugin can't load %s plugin symbol '%s'"
                , _dllName
                , symbol
            );

            return false;
        }

        TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

        if( this->createPlugin( dlib, dllCreatePlugin, true ) == false )
        {
            LOGGER_ERROR( "PluginService::loadPlugin can't load '%s' plugin [invalid create]"
                , _dllName
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::createPlugin( const DynamicLibraryInterfacePtr & _dlib, TPluginCreate _create, bool _dynamic )
    {
        if( _create == nullptr )
        {
            return false;
        }

        ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

        PluginInterface * plugin;
        if( _create( serviceProvider, &plugin, _dynamic ) == false )
        {
            LOGGER_ERROR( "PluginService::createPlugin can't create plugin [invalid create]"
            );

            return false;
        }

        if( plugin == nullptr )
        {
            LOGGER_ERROR( "PluginService::createPlugin can't create plugin [plugin is NULL]"
            );

            return false;
        }

        if( this->addPlugin( _dlib, plugin ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::addPlugin( const DynamicLibraryInterfacePtr & _dlib, const PluginInterfacePtr & _plugin )
    {
        if( _plugin == nullptr )
        {
            return false;
        }

        const Char * name = _plugin->getPluginName();

        if( this->hasPlugin( name ) == true )
        {
            return false;
        }

        if( _plugin->initializePlugin() == false )
        {
            LOGGER_ERROR( "PluginService::loadPlugin invalid initialize plugin '%s'"
                , name
            );

            return false;
        }

        PluginDesc desc;
        strcpy( desc.name, name );
        desc.dlib = _dlib;
        desc.plugin = _plugin;

        m_plugins.emplace_back( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::removePlugin( const PluginInterfacePtr & _plugin )
    {
        if( _plugin == nullptr )
        {
            return false;
        }

        for( VectorPlugins::iterator
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

            desc.plugin->finalizePlugin();

            m_plugins.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::hasPlugin( const Char * _name ) const
    {
        for( const PluginDesc & desc : m_plugins )
        {
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
    const PluginInterfacePtr & PluginService::getPlugin( const Char * _name ) const
    {
        for( const PluginDesc & desc : m_plugins )
        {
            if( desc.plugin == nullptr )
            {
                continue;
            }

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            const PluginInterfacePtr & plugin = desc.plugin;

            return plugin;
        }

        return PluginInterfacePtr::none();
    }
}