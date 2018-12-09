#include "PluginBase.h"

#include "Interface/ModuleServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_dynamicLoad( false )
        , m_initialize( false )
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
    bool PluginBase::avaliable()
    {
        bool successful = this->_avaliable();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::initialize()
    {
        if( m_initialize == true )
        {
            return true;
        }

        bool successful = this->_initialize();

        m_initialize = successful;

        return m_initialize;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::finalize()
    {
        if( m_initialize == false )
        {
            return;
        }

        m_initialize = false;

        for( const String & serviceName : m_dependencyServices )
        {
            SERVICE_PROVIDER_GET()
                ->finalizeService( serviceName.c_str() );
        }

        m_dependencyServices.clear();

        this->_finalize();

        for( const ConstString & moduleFactory : m_moduleFactories )
        {
            MODULE_SERVICE()
                ->unregisterModule( moduleFactory );
        }

        m_moduleFactories.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isInitialize() const
    {
        return m_initialize;
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
    bool PluginBase::_initialize()
    {
        ///Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::_finalize()
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