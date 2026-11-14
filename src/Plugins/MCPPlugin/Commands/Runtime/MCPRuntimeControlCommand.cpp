#include "MCPRuntimeControlCommand.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "../../Contexts/MCPRuntimeContext.h"
#include "MCPAdvanceFramesOperation.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const int64_t MCP_ADVANCE_MAX_FRAMES = 10000;
    //////////////////////////////////////////////////////////////////////////
    MCPRuntimeControlCommand::MCPRuntimeControlCommand( MCPRuntimeContext * _context )
        : m_context( _context )
    {
        ConstString pause = STRINGIZE_STRING_LOCAL( "pause" );
        ConstString resume = STRINGIZE_STRING_LOCAL( "resume" );
        ConstString timeScale = STRINGIZE_STRING_LOCAL( "time_scale" );
        ConstString advanceFrames = STRINGIZE_STRING_LOCAL( "advance_frames" );

        m_actions.emplace( pause, &MCPRuntimeControlCommand::pause_ );
        m_actions.emplace( resume, &MCPRuntimeControlCommand::resume_ );
        m_actions.emplace( timeScale, &MCPRuntimeControlCommand::timeScale_ );
        m_actions.emplace( advanceFrames, &MCPRuntimeControlCommand::advanceFrames_ );
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object actionObject;
        if( _request.params.exist( "action", &actionObject ) == false )
        {
            _response->errorMessage = "runtime control action is required";

            return EMCPCommandStatus::FAILURE;
        }

        if( actionObject.is_type_string() == false )
        {
            _response->errorMessage = "runtime control action must be a string";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * actionValue = actionObject;
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "runtime control action is not supported";

            return EMCPCommandStatus::FAILURE;
        }

        if( m_context->isAdvancing() == true )
        {
            _response->errorMessage = "a frame advance is running";

            return EMCPCommandStatus::FAILURE;
        }

        Action action = it->second;
        EMCPCommandStatus status = (this->*action)( _request, _response );

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::pause_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        m_context->setPaused( true );

        this->writeState_( "pause", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::resume_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        m_context->setPaused( false );

        this->writeState_( "resume", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::timeScale_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object valueObject;
        if( _request.params.exist( "value", &valueObject ) == false )
        {
            _response->errorMessage = "time_scale requires a non-negative value";

            return EMCPCommandStatus::FAILURE;
        }

        if( valueObject.is_type_integer() == false && valueObject.is_type_real() == false )
        {
            _response->errorMessage = "time_scale value must be a number";

            return EMCPCommandStatus::FAILURE;
        }

        float value = _request.params.get( "value", -1.f );

        if( StdMath::isfinite( value ) == false )
        {
            _response->errorMessage = "time_scale requires a finite value";

            return EMCPCommandStatus::FAILURE;
        }

        if( value < 0.f )
        {
            _response->errorMessage = "time_scale requires a non-negative value";

            return EMCPCommandStatus::FAILURE;
        }

        m_context->setTimeScale( value );

        this->writeState_( "time_scale", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::advanceFrames_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object framesObject;
        if( _request.params.exist( "frames", &framesObject ) == false )
        {
            _response->errorMessage = "advance_frames requires 1..10000 frames";

            return EMCPCommandStatus::FAILURE;
        }

        if( framesObject.is_type_integer() == false )
        {
            _response->errorMessage = "advance_frames frames must be an integer";

            return EMCPCommandStatus::FAILURE;
        }

        int64_t frameCount = framesObject;

        if( frameCount <= 0 )
        {
            _response->errorMessage = "advance_frames requires 1..10000 frames";

            return EMCPCommandStatus::FAILURE;
        }

        if( frameCount > MCP_ADVANCE_MAX_FRAMES )
        {
            _response->errorMessage = "advance_frames requires 1..10000 frames";

            return EMCPCommandStatus::FAILURE;
        }

        if( _request.params.exist( "frameTime", nullptr ) == true )
        {
            _response->errorMessage = "advance_frames uses value as a time factor, not frameTime";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object valueObject;
        if( _request.params.exist( "value", &valueObject ) == true )
        {
            if( valueObject.is_type_integer() == false && valueObject.is_type_real() == false )
            {
                _response->errorMessage = "advance_frames value must be a number";

                return EMCPCommandStatus::FAILURE;
            }
        }

        float timeFactor = _request.params.get( "value", 1.f );

        if( StdMath::isfinite( timeFactor ) == false )
        {
            _response->errorMessage = "advance frame time factor must be finite";

            return EMCPCommandStatus::FAILURE;
        }

        if( timeFactor < 0.f )
        {
            _response->errorMessage = "advance frame time factor cannot be negative";

            return EMCPCommandStatus::FAILURE;
        }

        uint32_t frames = (uint32_t)frameCount;
        MCPAdvanceFramesOperationPtr operation = Helper::makeFactorableUnique<MCPAdvanceFramesOperation>( MENGINE_DOCUMENT_FACTORABLE, m_context, frames, timeFactor );

        operation->begin();

        _response->operation = operation;

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::writeState_( const Char * _action, MCPCommandResponse * const _response ) const
    {
        bool updateFrozen = APPLICATION_SERVICE()
            ->isUpdateFrozen();

        bool runtimePaused = m_context->isPaused();

        float timeScale = TIMELINE_SERVICE()
            ->calcTimeFactor();

        _response->result.set( "action", _action );
        _response->result.set( "updateFrozen", updateFrozen );
        _response->result.set( "runtimePaused", runtimePaused );
        _response->result.set( "timeScale", timeScale );
    }
    //////////////////////////////////////////////////////////////////////////
}
