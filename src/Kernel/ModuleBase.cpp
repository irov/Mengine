#include "ModuleBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleBase::ModuleBase()
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
    bool ModuleBase::available()
    {
        bool successful = this->_available();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_available()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::initialize()
    {
        bool successful = this->_initialize();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::finalize()
    {
        this->_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleBase::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleBase::_finalize()
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