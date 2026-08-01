#include "MCPInputTouchCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputTouchCommand::MCPInputTouchCommand( MCPInputContext * _context )
        : m_context( _context )
    {
        ConstString move = STRINGIZE_STRING_LOCAL( "move" );
        ConstString start = STRINGIZE_STRING_LOCAL( "start" );
        ConstString end = STRINGIZE_STRING_LOCAL( "end" );
        ConstString cancel = STRINGIZE_STRING_LOCAL( "cancel" );

        m_actions.emplace( move, &MCPInputTouchCommand::move_ );
        m_actions.emplace( start, &MCPInputTouchCommand::start_ );
        m_actions.emplace( end, &MCPInputTouchCommand::end_ );
        m_actions.emplace( cancel, &MCPInputTouchCommand::end_ );
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputTouchCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * actionValue = _request.params.get( "action", "" );
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "unknown touch action";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object touchesObject;

        if( _request.params.exist( "touches", &touchesObject ) == false || touchesObject.is_type_array() == false )
        {
            _response->errorMessage = "touches must be an array";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * coordinateSpace = _request.params.get( "coordinateSpace", "normalized" );
        jpp::array touches( touchesObject );
        Timestamp timestamp = Helper::getSystemTimestamp();
        uint32_t eventCount = 0;
        Action action = it->second;

        for( const jpp::object & touch : touches )
        {
            uint32_t id = touch.get( "id", MENGINE_UINT32_C(0) );

            if( id >= MENGINE_INPUT_MAX_TOUCH )
            {
                _response->errorMessage = "touch id exceeds MENGINE_INPUT_MAX_TOUCH";

                return EMCPCommandStatus::FAILURE;
            }

            float x = touch.get( "x", 0.f );
            float y = touch.get( "y", 0.f );
            String error;
            bool normalized = m_context->normalizeCoordinates( coordinateSpace, &x, &y, &error );

            if( normalized == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            TouchEvent event;
            event.timestamp = timestamp;
            event.touchId = (ETouchCode)id;
            event.x = x;
            event.y = y;
            event.pressure = touch.get( "pressure", 1.f );
            (this->*action)( event );
            ++eventCount;
        }

        _response->result.set( "action", actionValue );
        _response->result.set( "events", eventCount );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputTouchCommand::move_( const TouchEvent & _event )
    {
        Helper::pushMouseMoveEvent( _event.timestamp, _event.touchId, _event.x, _event.y, 0.f, 0.f, _event.pressure, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputTouchCommand::start_( const TouchEvent & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, _event.touchId, _event.x, _event.y, MC_LBUTTON, _event.pressure, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputTouchCommand::end_( const TouchEvent & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, _event.touchId, _event.x, _event.y, MC_LBUTTON, _event.pressure, false );
    }
    //////////////////////////////////////////////////////////////////////////
}
