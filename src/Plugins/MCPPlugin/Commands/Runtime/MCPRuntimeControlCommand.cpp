#include "MCPRuntimeControlCommand.h"

#include "../../Contexts/MCPRuntimeContext.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPRuntimeControlCommand::MCPRuntimeControlCommand( MCPCommandHostInterface * _host, MCPRuntimeContext * _context )
        : m_host( _host )
        , m_context( _context )
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
        const Char * actionValue = _request.params.get( "action", "" );
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "runtime control action is not supported";

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

        if( m_context->isPaused() == false )
        {
            float timeFactorBase = TIMELINE_SERVICE()->getTimeFactorBase();
            int32_t timeFactorCount = TIMELINE_SERVICE()->getTimeFactorCount();

            m_context->setPreviousTimeFactorBase( timeFactorBase );
            m_context->setPreviousTimeFactorCount( timeFactorCount );
            TIMELINE_SERVICE()->setTimeFactorCount( 0 );
            TIMELINE_SERVICE()->setTimeFactorBase( 0.f );
            m_context->setPaused( true );
        }

        this->writeState_( "pause", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::resume_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        m_context->restoreRuntime();
        this->writeState_( "resume", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::timeScale_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        float value = _request.params.get( "value", -1.f );

        if( value < 0.f )
        {
            _response->errorMessage = "time_scale requires a non-negative value";

            return EMCPCommandStatus::FAILURE;
        }

        if( m_context->isPaused() == true )
        {
            m_context->setPreviousTimeFactorBase( value );
            m_context->setPreviousTimeFactorCount( 0 );
        }
        else
        {
            TIMELINE_SERVICE()->setTimeFactorCount( 0 );
            TIMELINE_SERVICE()->setTimeFactorBase( value );
        }

        this->writeState_( "time_scale", _response );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPRuntimeControlCommand::advanceFrames_( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        if( m_pendingAdvance.requestId != 0 )
        {
            _response->errorMessage = "another frame advance is already running";

            return EMCPCommandStatus::FAILURE;
        }

        uint32_t frames = _request.params.get( "frames", MENGINE_UINT32_C(0) );

        if( frames == 0 || frames > 10000 )
        {
            _response->errorMessage = "advance_frames requires 1..10000 frames";

            return EMCPCommandStatus::FAILURE;
        }

        float timeFactor = _request.params.get( "value", 1.f );

        if( timeFactor < 0.f )
        {
            _response->errorMessage = "advance frame time factor cannot be negative";

            return EMCPCommandStatus::FAILURE;
        }

        m_pendingAdvance.requestId = _request.requestId;
        m_pendingAdvance.startGeneration = m_host->getMCPUpdateGeneration();
        m_pendingAdvance.frames = frames;
        m_pendingAdvance.restoreTimeFactorBase = TIMELINE_SERVICE()->getTimeFactorBase();
        m_pendingAdvance.restoreTimeFactorCount = TIMELINE_SERVICE()->getTimeFactorCount();

        TIMELINE_SERVICE()->setTimeFactorCount( 0 );
        TIMELINE_SERVICE()->setTimeFactorBase( timeFactor );

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::writeState_( const Char * _action, MCPCommandResponse * const _response ) const
    {
        bool updateFrozen = APPLICATION_SERVICE()->isUpdateFrozen();
        bool runtimePaused = m_context->isPaused();
        float timeScale = TIMELINE_SERVICE()->calcTimeFactor();

        _response->result.set( "action", _action );
        _response->result.set( "updateFrozen", updateFrozen );
        _response->result.set( "runtimePaused", runtimePaused );
        _response->result.set( "timeScale", timeScale );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::update()
    {
        if( m_pendingAdvance.requestId == 0 )
        {
            return;
        }

        uint64_t generation = m_host->getMCPUpdateGeneration();

        if( generation - m_pendingAdvance.startGeneration < m_pendingAdvance.frames )
        {
            return;
        }

        jpp::object result = jpp::make_object();
        result.set( "action", "advance_frames" );
        result.set( "frames", m_pendingAdvance.frames );
        result.set( "completed", true );

        uint32_t requestId = m_pendingAdvance.requestId;
        this->restoreAdvance_();
        m_host->sendMCPCommandResponse( requestId, result );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::cancel( uint32_t _requestId )
    {
        if( m_pendingAdvance.requestId != _requestId )
        {
            return;
        }

        this->restoreAdvance_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::disconnect()
    {
        this->restoreAdvance_();
        m_context->restoreRuntime();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeControlCommand::restoreAdvance_()
    {
        if( m_pendingAdvance.requestId == 0 )
        {
            return;
        }

        TIMELINE_SERVICE()->setTimeFactorBase( m_pendingAdvance.restoreTimeFactorBase );
        TIMELINE_SERVICE()->setTimeFactorCount( m_pendingAdvance.restoreTimeFactorCount );
        m_pendingAdvance = PendingAdvance();
    }
    //////////////////////////////////////////////////////////////////////////
}
