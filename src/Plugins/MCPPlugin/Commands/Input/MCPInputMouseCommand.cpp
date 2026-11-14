#include "MCPInputMouseCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputMouseCommand::MCPInputMouseCommand( MCPInputContext * _context, MCPSceneContext * _sceneContext )
        : m_context( _context )
        , m_sceneContext( _sceneContext )
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

        const Char * coordinateSpace = _request.params.get( "coordinateSpace", "normalized" );

        mt::vec2f point;
        String error;
        if( m_context->resolvePoint( m_sceneContext, _request.params, coordinateSpace, &point, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        const Char * buttonValue = _request.params.get( "button", "left" );

        EMouseButtonCode button;
        if( m_context->getMouseButton( buttonValue, &button ) == false )
        {
            _response->errorMessage = "unknown mouse button, expected left, middle, right, x1 or x2";

            return EMCPCommandStatus::FAILURE;
        }

        VectorMCPKeyCodes modifiers;
        if( m_context->getModifierKeys( _request.params, &modifiers, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        float previousX = m_context->getCursorX();
        float previousY = m_context->getCursorY();

        MouseEventDesc event;
        m_context->getSpecial( modifiers, &event.special );

        event.timestamp = Helper::getSystemTimestamp();
        event.x = point.x;
        event.y = point.y;
        event.dx = point.x - previousX;
        event.dy = point.y - previousY;
        event.button = button;
        event.wheel = _request.params.get( "wheel", 0 );
        m_context->setCursor( point.x, point.y );

        Action action = it->second;

        if( action != &MCPInputMouseCommand::up_ )
        {
            m_context->pushModifierKeys( modifiers, true );
        }

        (this->*action)( event );

        if( action != &MCPInputMouseCommand::down_ )
        {
            m_context->pushModifierKeys( modifiers, false );
        }

        _response->result.set( "action", actionValue );
        _response->result.set( "x", point.x );
        _response->result.set( "y", point.y );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::move_( const MouseEventDesc & _event )
    {
        Helper::pushMouseMoveEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.dx, _event.dy, 0.f, 0.f, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::down_( const MouseEventDesc & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 1.f, true, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::up_( const MouseEventDesc & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 0.f, false, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::click_( const MouseEventDesc & _event )
    {
        this->down_( _event );
        this->up_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::doubleClick_( const MouseEventDesc & _event )
    {
        this->click_( _event );
        this->click_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::wheel_( const MouseEventDesc & _event )
    {
        Helper::pushMouseWheelEvent( _event.timestamp, _event.x, _event.y, 0.f, WC_CENTRAL, _event.wheel, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
}
