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

        const ConstString & name = this->getName();

        NOTIFICATION_NOTIFY( NOTIFICATOR_MODULE_INITIALIZE, name );

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

        const ConstString & name = this->getName();

        NOTIFICATION_NOTIFY( NOTIFICATOR_MODULE_FINALIZE, name );

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
    void ModuleBase::beginUpdate( bool _focus )
    {
        this->_beginUpdate( _focus );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::endUpdate()
    {
        this->_endUpdate();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_endUpdate()
    {
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
    bool ModuleBase::handleKeyEvent( const InputKeyEvent & _event )
    {
        bool handle = this->_handleKeyEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleTextEvent( const InputTextEvent & _event )
    {
        bool handle = this->_handleTextEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////    
    bool ModuleBase::_handleKeyEvent( const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////    
    bool ModuleBase::_handleTextEvent( const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        bool handle = this->_handleAccelerometerEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        bool handle = this->_handleMouseButtonEvent( _event );

        return handle;

    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        bool handle = this->_handleMouseButtonEventBegin( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        bool handle = this->_handleMouseButtonEventEnd( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool handle = this->_handleMouseMove( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = this->_handleMouseWheel( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseMove( const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        bool handle = this->_handleMouseEnter( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        this->_handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}