#include "PluginBase.h"

#include "Kernel/Logger.h"

#include "Interface/ModuleServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_dynamicLoad( false )
        , m_initializePlugin( false )
        , m_availablePlugin( true )
        , m_systemPlugin( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginBase::~PluginBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::setDynamicLoad( bool _dynamicLoad )
    {
        m_dynamicLoad = _dynamicLoad;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isDynamicLoad() const
    {
        return m_dynamicLoad;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_availablePlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_systemPlugin() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::initializePlugin()
    {
        if( m_initializePlugin == true )
        {
            return true;
        }

        m_availablePlugin = this->_availablePlugin();

        if( m_availablePlugin == false )
        {
            LOGGER_WARNING( "plugin '%s' not available"
                , this->getPluginName()
            );

            return true;
        }

        bool successful = this->_initializePlugin();

        if( successful == false )
        {
            LOGGER_WARNING( "plugin '%s' not initialize"
                , this->getPluginName()
            );

            return false;
        }

        m_initializePlugin = successful;

        m_systemPlugin = this->_systemPlugin();

        return m_initializePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::finalizePlugin()
    {
        if( m_initializePlugin == false )
        {
            return;
        }

        m_initializePlugin = false;

        for( const String & serviceName : m_dependencyServices )
        {
            SERVICE_PROVIDER_GET()
                ->finalizeService( serviceName.c_str() );
        }

        m_dependencyServices.clear();

        this->_finalizePlugin();

        for( const ConstString & moduleFactory : m_moduleFactories )
        {
            MODULE_SERVICE()
                ->unregisterModule( moduleFactory );
        }

        m_moduleFactories.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isInitializePlugin() const
    {
        return m_initializePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isAvailablePlugin() const
    {
        return m_availablePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isSystemPlugin() const
    {
        return m_systemPlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::destroy()
    {
        this->_destroy();

        bool dynamicLoad = m_dynamicLoad;

        delete this;

        if( dynamicLoad == true )
        {
            stdex_allocator_finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_initializePlugin()
    {
        ///Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::_finalizePlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::addDependencyService( const Char * _name )
    {
        if( SERVICE_PROVIDER_GET()
            ->existService( _name ) == false )
        {
            return false;
        }

        m_dependencyServices.emplace_back( _name );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory )
    {
        if( MODULE_SERVICE()
            ->registerModule( _name, _factory ) == false )
        {
            return false;
        }

        m_moduleFactories.emplace_back( _name );

        return true;
    }
}