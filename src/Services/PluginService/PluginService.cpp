#include "PluginService.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/StdString.h"

#include <algorithm>

#ifndef MENGINE_PLUGIN_CREATE_FUNCTION_NAME
#define MENGINE_PLUGIN_CREATE_FUNCTION_NAME "dllCreatePlugin"
#endif

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
        this->unloadPlugins();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::loadPlugin( const Char * _dynamicLibraryName, const DocumentPtr & _doc )
    {
        LOGGER_MESSAGE_RELEASE( "load plugin '%s'"
            , _dynamicLibraryName
        );

        DynamicLibraryInterfacePtr dynamicLibrary = PLATFORM_SERVICE()
            ->loadDynamicLibrary( _dynamicLibraryName, _doc );

        if( dynamicLibrary == nullptr )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid load]"
                , _dynamicLibraryName
            );

            return false;
        }

        const Char * symbol = MENGINE_PLUGIN_CREATE_FUNCTION_NAME;

        TDynamicLibraryFunction function_dllCreatePlugin = dynamicLibrary->getSymbol( symbol );

        if( function_dllCreatePlugin == nullptr )
        {
            LOGGER_ERROR( "can't load '%s' plugin symbol '%s'"
                , _dynamicLibraryName
                , symbol
            );

            return false;
        }

        TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

        if( this->createPlugin( dynamicLibrary, dllCreatePlugin, true, _doc ) == false )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid create]"
                , _dynamicLibraryName
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::createPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, TPluginCreate _create, bool _dynamic, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _create == nullptr )
        {
            return false;
        }

        UniqueId id = GENERATE_UNIQUE_IDENTITY();

        ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

        PluginInterface * plugin;
        if( _create( serviceProvider, &plugin, id, _dynamic ) == false )
        {
            LOGGER_ERROR( "can't create plugin [invalid create]" );

            return false;
        }

        if( plugin == nullptr )
        {
            LOGGER_ERROR( "can't create plugin [plugin is nullptr]" );

            return false;
        }

#if MENGINE_DOCUMENT_ENABLE
        plugin->setDocument( _doc );
#endif

        if( this->addPlugin( _dynamicLibrary, PluginInterfacePtr::from( plugin ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::unloadPlugins()
    {
        VectorPlugins reverse_plugins = std::move( m_plugins );
        std::reverse( reverse_plugins.begin(), reverse_plugins.end() );

        for( PluginDesc & desc : reverse_plugins )
        {
            if( desc.dynamicLibrary != nullptr )
            {
                continue;
            }

            if( desc.plugin->isSystemPlugin() == true &&
                this->isInitializeService() == true )
            {
                m_plugins.emplace_back( desc );

                continue;
            }

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;

            desc.dynamicLibrary = nullptr;
        }

        for( PluginDesc & desc : reverse_plugins )
        {
            if( desc.dynamicLibrary == nullptr )
            {
                continue;
            }

            if( desc.plugin->isSystemPlugin() == true &&
                this->isInitializeService() == true )
            {
                m_plugins.emplace_back( desc );

                continue;
            }

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::addPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, const PluginInterfacePtr & _plugin )
    {
        if( _plugin == nullptr )
        {
            return false;
        }

        const Char * name = _plugin->getPluginName();

        if( this->hasPlugin( name ) == true )
        {
            LOGGER_ERROR( "alredy exist plugin '%s'"
                , name
            );

            return false;
        }

        if( _plugin->initializePlugin() == false )
        {
            LOGGER_ERROR( "invalid initialize plugin '%s'"
                , name
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( name ) < MENGINE_PLUGIN_NAME_MAX );

        PluginDesc desc;
        MENGINE_STRCPY( desc.name, name );
        desc.dynamicLibrary = _dynamicLibrary;
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
            desc.plugin = nullptr;

            desc.dynamicLibrary = nullptr;

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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                continue;
            }

            const PluginInterfacePtr & plugin = desc.plugin;

            return plugin;
        }

        return PluginInterfacePtr::none();
    }
}