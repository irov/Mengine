#include "ModuleBase.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleBase::ModuleBase()
        : m_availableModule( false )
        , m_initializeModule( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleBase::~ModuleBase()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "module '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ModuleBase::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::isAvailableModule() const
    {
        return m_availableModule;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::isInitializeModule() const
    {
        return m_initializeModule;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_availableModule() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::initializeModule()
    {
        LOGGER_INFO( "module", "initialize '%s'"
            , this->getName().c_str()
        );

        m_availableModule = this->_availableModule();

        if( m_availableModule == false )
        {
            return true;
        }

        bool successful = this->_initializeModule();

        if( successful == false )
        {
            return false;
        }

        m_initializeModule = true;

        NOTIFICATION_NOTIFY( NOTIFICATOR_MODULE_INITIALIZE, m_name );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::finalizeModule()
    {
        LOGGER_INFO( "module", "finalize '%s'"
            , this->getName().c_str()
        );

        if( m_availableModule == false )
        {
            return;
        }

        if( m_initializeModule == false )
        {
            return;
        }

        m_initializeModule = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_MODULE_FINALIZE, m_name );

        this->_finalizeModule();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_initializeModule()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_finalizeModule()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::update( bool _focus )
    {
        this->_update( _focus );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        this->_render( _renderPipeline, _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::message( const ConstString & _messageName, const MapWParams & _params )
    {
        this->_message( _messageName, _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::messageAll( const ConstString & _messageName, const MapWParams & _params )
    {
        this->_messageAll( _messageName, _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_message( const ConstString & _messageName, const MapWParams & _params )
    {
        MENGINE_UNUSED( _messageName );
        MENGINE_UNUSED( _params );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_messageAll( const ConstString & _messageName, const MapWParams & _params )
    {
        MENGINE_UNUSED( _messageName );
        MENGINE_UNUSED( _params );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}