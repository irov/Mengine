#include "PluginService.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

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
        m_pluginsMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::_finalizeService()
    {
        this->unloadPlugins();

        m_pluginsMutex = nullptr;

        m_availablePlugins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::loadPlugin( const Char * _dynamicLibraryName, bool _frameworkMode, const DocumentInterfacePtr & _doc )
    {
        DynamicLibraryInterfacePtr dynamicLibrary = PLATFORM_SERVICE()
            ->loadDynamicLibrary( _dynamicLibraryName, _doc );

        if( dynamicLibrary == nullptr )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid load]"
                , _dynamicLibraryName
            );

            return false;
        }

        const Char * symbol = PLUGIN_FACTORY_CREATE_FUNCTION_NAME;

        TDynamicLibraryFunction function_dllMengineCreatePlugin = dynamicLibrary->getSymbol( symbol );

        if( function_dllMengineCreatePlugin == nullptr )
        {
            LOGGER_ERROR( "can't load '%s' plugin symbol '%s'"
                , _dynamicLibraryName
                , symbol
            );

            return false;
        }

        TPluginCreate dllMengineCreatePlugin = (TPluginCreate)function_dllMengineCreatePlugin;

        if( this->createPlugin( dynamicLibrary, dllMengineCreatePlugin, true, _frameworkMode, _doc ) == false )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid create]"
                , _dynamicLibraryName
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::createPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, TPluginCreate _create, bool _dynamicLoad, bool _frameworkMode, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = Helper::generateUniqueIdentity();

        ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

        PluginInterface * plugin;
        if( _create( serviceProvider, &plugin, id, _dynamicLoad, _frameworkMode ) == false )
        {
            LOGGER_ERROR( "can't create plugin [invalid create]" );

            return false;
        }

        if( plugin == nullptr )
        {
            LOGGER_ERROR( "can't create plugin [plugin is nullptr]" );

            return false;
        }

#if defined(MENGINE_DOCUMENT_ENABLE)
        plugin->setDocument( _doc );
#endif

        PluginInterfacePtr plugin_ptr = PluginInterfacePtr::from( plugin );

        if( this->addPlugin( plugin_ptr, _dynamicLibrary ) == false )
        {
            LOGGER_ERROR( "invalid create plugin '%s'"
                , plugin->getPluginName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::unloadPlugins()
    {
        VectorPlugins reverse_plugins;
        std::swap( reverse_plugins, m_plugins );

        StdAlgorithm::reverse( reverse_plugins.begin(), reverse_plugins.end() );

        for( PluginDesc & desc : reverse_plugins )
        {
            if( desc.dynamicLibrary != nullptr )
            {
                continue;
            }

            if( desc.plugin->isSystemPlugin() == true && this->isInitializeService() == true )
            {
                m_plugins.push_back( desc );

                desc.plugin = nullptr;

                continue;
            }

            if( desc.plugin->isFrameworkMode() == true && this->isInitializeService() == true )
            {
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

            if( desc.plugin->isSystemPlugin() == true && this->isInitializeService() == true )
            {
                m_plugins.push_back( desc );

                desc.plugin = nullptr;

                continue;
            }

            if( desc.plugin->isFrameworkMode() == true && this->isInitializeService() == true )
            {
                continue;
            }

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;
        }

        for( PluginDesc & desc : reverse_plugins )
        {
            if( desc.plugin == nullptr )
            {
                continue;
            }

            if( desc.dynamicLibrary != nullptr )
            {
                continue;
            }

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;

            desc.dynamicLibrary = nullptr;
        }

        for( PluginDesc & desc : reverse_plugins )
        {
            if( desc.plugin == nullptr )
            {
                continue;
            }

            if( desc.dynamicLibrary == nullptr )
            {
                continue;
            }

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;
        }            
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::addPlugin( const PluginInterfacePtr & _plugin, const DynamicLibraryInterfacePtr & _dynamicLibrary )
    {
        if( _plugin == nullptr )
        {
            return false;
        }

        const ConstString & pluginName = _plugin->getPluginName();

        if( this->hasPlugin( pluginName ) == true )
        {
            LOGGER_ERROR( "already exist plugin '%s'"
                , _plugin->getPluginName().c_str()
            );

            return false;
        }

        if( this->autoRegistration_( _plugin ) == false )
        {
            LOGGER_ERROR( "invalid auto registration plugin '%s'"
                , _plugin->getPluginName().c_str()
            );

            return false;
        }

        if( _plugin->initializePlugin() == false )
        {
            LOGGER_ERROR( "invalid initialize plugin '%s'"
                , _plugin->getPluginName().c_str()
            );

            return false;
        }

        bool available = _plugin->isAvailablePlugin();

        this->setAvailablePlugin( pluginName, available );

        PluginDesc desc;
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

            if( desc.plugin != _plugin )
            {
                continue;
            }

            const ConstString & pluginName = _plugin->getPluginName();

            this->setAvailablePlugin( pluginName, false );

            desc.plugin->finalizePlugin();
            desc.plugin = nullptr;

            desc.dynamicLibrary = nullptr;

            m_plugins.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::hasPlugin( const ConstString & _pluginName ) const
    {
        for( const PluginDesc & desc : m_plugins )
        {
            if( desc.plugin == nullptr )
            {
                continue;
            }

            const ConstString & pluginName = desc.plugin->getPluginName();

            if( pluginName != _pluginName )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const PluginInterfacePtr & PluginService::getPlugin( const ConstString & _pluginName ) const
    {
        for( const PluginDesc & desc : m_plugins )
        {
            if( desc.plugin == nullptr )
            {
                continue;
            }

            const ConstString & pluginName = desc.plugin->getPluginName();

            if( pluginName != _pluginName )
            {
                continue;
            }

            const PluginInterfacePtr & plugin = desc.plugin;

            return plugin;
        }

        return PluginInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginService::setAvailablePlugin( const ConstString & _pluginName, bool _available )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        m_availablePlugins.insert_or_assign( _pluginName, _available );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::isAvailablePlugin( const ConstString & _pluginName ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MapAvailablePlugins::const_iterator it_found = m_availablePlugins.find( _pluginName );

        if( it_found == m_availablePlugins.end() )
        {
            return false;
        }

        bool available = it_found->second;

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginService::autoRegistration_( const PluginInterfacePtr & _plugin )
    {
        for( const PluginDesc & desc : m_plugins )
        {
            const PluginInterfacePtr & plugin = desc.plugin;

            bool available = plugin->isAvailablePlugin();

            if( available == false )
            {
                continue;
            }

            if( plugin->registerPlugin( _plugin ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
