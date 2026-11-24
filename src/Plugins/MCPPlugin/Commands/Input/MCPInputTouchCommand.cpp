#include "MCPInputTouchCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputTouchCommand::MCPInputTouchCommand( MCPInputContext * _context, MCPSceneContext * _sceneContext )
        : m_context( _context )
        , m_sceneContext( _sceneContext )
    {
        ConstString move = STRINGIZE_STRING_LOCAL( "move" );
        ConstString start = STRINGIZE_STRING_LOCAL( "start" );
        ConstString end = STRINGIZE_STRING_LOCAL( "end" );
        ConstString cancel = STRINGIZE_STRING_LOCAL( "cancel" );

        m_actions.emplace( move, ETA_MOVE );
        m_actions.emplace( start, ETA_START );
        m_actions.emplace( end, ETA_END );
        m_actions.emplace( cancel, ETA_CANCEL );
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
        ETouchAction action = it->second;
        bool touchMoved = false;

        for( const jpp::object & touch : touches )
        {
            uint32_t id = touch.get( "id", MENGINE_UINT32_C(0) );

            if( id >= MENGINE_INPUT_MAX_TOUCH )
            {
                _response->errorMessage = "touch id exceeds MENGINE_INPUT_MAX_TOUCH";

                break;
            }

            mt::vec2f point;
            String error;
            if( m_context->resolvePoint( m_sceneContext, touch, coordinateSpace, &point, &error ) == false )
            {
                _response->errorMessage = error;

                break;
            }

            ETouchCode touchId = static_cast<ETouchCode>(id);
            float pressure = touch.get( "pressure", 1.f );
            ++eventCount;

            if( action == ETA_MOVE )
            {
                Helper::pushMouseMoveEvent( timestamp, touchId, point.x, point.y, 0.f, 0.f, pressure, 0.f, true );

                touchMoved = true;

                continue;
            }

            bool down = action == ETA_START;
            bool cancelled = action == ETA_CANCEL;

            Helper::pushMouseButtonEvent( timestamp, touchId, point.x, point.y, MC_LBUTTON, pressure, down, cancelled, true );
        }

        if( touchMoved == true )
        {
            Helper::pushTouchMoveEndEvent( timestamp );
        }

        if( _response->errorMessage.empty() == false )
        {
            return EMCPCommandStatus::FAILURE;
        }

        _response->result.set( "action", actionValue );
        _response->result.set( "events", eventCount );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
