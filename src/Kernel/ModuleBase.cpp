#include "ModuleBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleBase::ModuleBase()
        : m_available( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleBase::~ModuleBase()
    {
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
        return m_available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_availableModule() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::initializeModule()
    {
        m_available = this->_availableModule();

        if( m_available == false )
        {
            return false;
        }

        bool successful = this->_initializeModule();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::finalizeModule()
    {
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
        (void)_focus;

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::tick( const UpdateContext * _context )
    {
        this->_tick( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::render( const RenderContext * _state )
    {
        this->_render( _state );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_tick( const UpdateContext * _context )
    {
        (void)_context;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_render( const RenderContext * _state )
    {
        (void)_state;

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
        (void)_messageName;
        (void)_params;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_messageAll( const ConstString & _messageName, const MapWParams & _params )
    {
        (void)_messageName;
        (void)_params;

        //Empty
    }
}