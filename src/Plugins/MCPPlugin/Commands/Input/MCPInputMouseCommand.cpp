#include "MCPInputMouseCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputMouseCommand::MCPInputMouseCommand( MCPInputContext * _context )
        : m_context( _context )
    {
        ConstString move = STRINGIZE_STRING_LOCAL( "move" );
        ConstString down = STRINGIZE_STRING_LOCAL( "down" );
        ConstString up = STRINGIZE_STRING_LOCAL( "up" );
        ConstString click = STRINGIZE_STRING_LOCAL( "click" );
        ConstString doubleClick = STRINGIZE_STRING_LOCAL( "double_click" );
        ConstString wheel = STRINGIZE_STRING_LOCAL( "wheel" );

        m_actions.emplace( move, &MCPInputMouseCommand::move_ );
        m_actions.emplace( down, &MCPInputMouseCommand::down_ );
        m_actions.emplace( up, &MCPInputMouseCommand::up_ );
        m_actions.emplace( click, &MCPInputMouseCommand::click_ );
        m_actions.emplace( doubleClick, &MCPInputMouseCommand::doubleClick_ );
        m_actions.emplace( wheel, &MCPInputMouseCommand::wheel_ );
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputMouseCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * actionValue = _request.params.get( "action", "" );
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "unknown mouse action";

            return EMCPCommandStatus::FAILURE;
        }

        float previousX = m_context->getCursorX();
        float previousY = m_context->getCursorY();
        float x = _request.params.get( "x", previousX );
        float y = _request.params.get( "y", previousY );
        const Char * coordinateSpace = _request.params.get( "coordinateSpace", "normalized" );
        String error;
        bool normalized = m_context->normalizeCoordinates( coordinateSpace, &x, &y, &error );

        if( normalized == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        const Char * buttonValue = _request.params.get( "button", "left" );
        MouseEvent event;
        event.timestamp = Helper::getSystemTimestamp();
        event.x = x;
        event.y = y;
        event.dx = x - previousX;
        event.dy = y - previousY;
        event.button = m_context->getMouseButton( buttonValue );
        event.wheel = _request.params.get( "wheel", 0 );
        m_context->setCursor( x, y );

        Action action = it->second;
        (this->*action)( event );

        _response->result.set( "action", actionValue );
        _response->result.set( "x", x );
        _response->result.set( "y", y );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::move_( const MouseEvent & _event )
    {
        Helper::pushMouseMoveEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.dx, _event.dy, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::down_( const MouseEvent & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 1.f, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::up_( const MouseEvent & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 0.f, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::click_( const MouseEvent & _event )
    {
        this->down_( _event );
        this->up_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::doubleClick_( const MouseEvent & _event )
    {
        this->click_( _event );
        this->click_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::wheel_( const MouseEvent & _event )
    {
        Helper::pushMouseWheelEvent( _event.timestamp, _event.x, _event.y, 0.f, WC_CENTRAL, _event.wheel );
    }
    //////////////////////////////////////////////////////////////////////////
}
