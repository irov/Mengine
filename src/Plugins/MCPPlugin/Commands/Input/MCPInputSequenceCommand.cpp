#include "MCPInputSequenceCommand.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputSequenceCommand::MCPInputSequenceCommand( MCPCommandHostInterface * _host, MCPInputSequenceStepRegistry * _registry )
        : m_host( _host )
        , m_registry( _registry )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputSequenceCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        if( m_pendingSequence.requestId != 0 )
        {
            _response->errorMessage = "another input sequence is already running";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object steps;

        if( _request.params.exist( "steps", &steps ) == false || steps.is_type_array() == false || steps.empty() == true )
        {
            _response->errorMessage = "input_sequence steps must be a non-empty array";

            return EMCPCommandStatus::FAILURE;
        }

        m_pendingSequence.requestId = _request.requestId;
        m_pendingSequence.startGeneration = m_host->getMCPUpdateGeneration();
        m_pendingSequence.steps = steps;

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputSequenceCommand::update()
    {
        if( m_pendingSequence.requestId == 0 )
        {
            return;
        }

        if( m_host->isMCPConnected() == false )
        {
            m_pendingSequence = PendingSequence();

            return;
        }

        uint32_t requestId = m_pendingSequence.requestId;
        if( m_host->isMCPDebuggerPaused() == true )
        {
            m_host->sendMCPCommandError( requestId, "runtime_paused", "runtime paused while an input sequence was running" );
            m_pendingSequence = PendingSequence();

            return;
        }

        uint64_t currentGeneration = m_host->getMCPUpdateGeneration();

        if( m_pendingSequence.resumeGeneration != 0 )
        {
            if( currentGeneration < m_pendingSequence.resumeGeneration )
            {
                return;
            }

            m_pendingSequence.resumeGeneration = 0;
        }

        if( m_pendingSequence.resumeTimestamp != 0 )
        {
            Timestamp timestamp = Helper::getSystemTimestamp();

            if( timestamp < m_pendingSequence.resumeTimestamp )
            {
                return;
            }

            m_pendingSequence.resumeTimestamp = 0;
        }

        jpp::array steps( m_pendingSequence.steps );

        if( m_pendingSequence.index >= steps.size() )
        {
            uint64_t framesElapsed = currentGeneration - m_pendingSequence.startGeneration;
            size_t stepCountValue = steps.size();
            uint32_t stepCount = (uint32_t)stepCountValue;
            jpp::object result = jpp::make_object();
            result.set( "completed", true );
            result.set( "steps", stepCount );
            result.set( "framesElapsed", (int64_t)framesElapsed );
            m_host->sendMCPCommandResponse( requestId, result );
            m_pendingSequence = PendingSequence();

            return;
        }

        jpp::object step = steps[m_pendingSequence.index];

        if( step.is_type_object() == false )
        {
            m_host->sendMCPCommandError( requestId, "execution_error", "input sequence step must be an object" );
            m_pendingSequence = PendingSequence();

            return;
        }

        const Char * typeValue = step.get( "type", "" );
        ConstString typeName = Helper::stringizeString( typeValue );
        MCPInputSequenceStepInterfacePtr stepCommand = m_registry->findStep( typeName );

        if( stepCommand == nullptr )
        {
            m_host->sendMCPCommandError( requestId, "unsupported", "unsupported input sequence step type" );
            m_pendingSequence = PendingSequence();

            return;
        }

        MCPInputSequenceStepRequest stepRequest = {step, m_pendingSequence.startGeneration, currentGeneration};
        MCPInputSequenceStepResponse stepResponse;
        bool successful = stepCommand->execute( stepRequest, &stepResponse );

        if( successful == false )
        {
            const Char * errorCode = stepResponse.errorCode.empty() == true ? "execution_error" : stepResponse.errorCode.c_str();
            const Char * errorMessage = stepResponse.errorMessage.c_str();
            m_host->sendMCPCommandError( requestId, errorCode, errorMessage );
            m_pendingSequence = PendingSequence();

            return;
        }

        if( stepResponse.completed == false )
        {
            return;
        }

        m_pendingSequence.resumeGeneration = stepResponse.resumeGeneration;
        m_pendingSequence.resumeTimestamp = stepResponse.resumeTimestamp;
        ++m_pendingSequence.index;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputSequenceCommand::cancel( uint32_t _requestId )
    {
        if( m_pendingSequence.requestId != _requestId )
        {
            return;
        }

        m_pendingSequence = PendingSequence();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputSequenceCommand::disconnect()
    {
        m_pendingSequence = PendingSequence();
    }
    //////////////////////////////////////////////////////////////////////////
}
