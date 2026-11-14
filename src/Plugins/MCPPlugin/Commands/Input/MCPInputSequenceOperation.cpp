#include "MCPInputSequenceOperation.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputSequenceOperation::MCPInputSequenceOperation( MCPCommandHostInterface * _host, MCPInputSequenceStepRegistry * _registry, const jpp::object & _steps, uint64_t _startFrame )
        : m_host( _host )
        , m_registry( _registry )
        , m_steps( _steps )
        , m_startFrame( _startFrame )
        , m_stepStartFrame( _startFrame )
        , m_resumeFrame( 0 )
        , m_resumeTimestamp( 0 )
        , m_index( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPInputSequenceOperation::~MCPInputSequenceOperation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputSequenceOperation::update( MCPCommandResponse * const _response )
    {
        if( m_host->isMCPDebuggerPaused() == true )
        {
            _response->errorCode = "runtime_paused";
            _response->errorMessage = "runtime paused while an input sequence was running";

            return EMCPCommandStatus::FAILURE;
        }

        uint64_t currentFrame = m_host->getMCPUpdateFrame();

        if( currentFrame < m_resumeFrame )
        {
            return EMCPCommandStatus::DEFERRED;
        }

        if( m_resumeTimestamp != 0 )
        {
            Timestamp timestamp = Helper::getSystemTimestamp();

            if( timestamp < m_resumeTimestamp )
            {
                return EMCPCommandStatus::DEFERRED;
            }

            m_resumeTimestamp = 0;
        }

        jpp::array steps( m_steps );

        if( m_index >= steps.size() )
        {
            uint64_t framesElapsed = currentFrame - m_startFrame;
            size_t stepCount = steps.size();

            _response->result.set( "completed", true );
            _response->result.set( "steps", (uint32_t)stepCount );
            _response->result.set( "framesElapsed", (int64_t)framesElapsed );

            return EMCPCommandStatus::SUCCESS;
        }

        jpp::object step = steps[m_index];

        if( step.is_type_object() == false )
        {
            _response->errorMessage = "input sequence step must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object typeObject;
        if( step.exist( "type", &typeObject ) == false )
        {
            _response->errorMessage = "input sequence step type must be a string";

            return EMCPCommandStatus::FAILURE;
        }

        if( typeObject.is_type_string() == false )
        {
            _response->errorMessage = "input sequence step type must be a string";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * typeValue = typeObject;
        ConstString typeName = Helper::stringizeString( typeValue );
        MCPInputSequenceStepInterfacePtr stepCommand = m_registry->findStep( typeName );

        if( stepCommand == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "unsupported input sequence step type";

            return EMCPCommandStatus::FAILURE;
        }

        MCPInputSequenceStepRequest stepRequest = {step, m_stepStartFrame, currentFrame};
        MCPInputSequenceStepResponse stepResponse;
        if( stepCommand->execute( stepRequest, &stepResponse ) == false )
        {
            _response->errorCode = stepResponse.errorCode;
            _response->errorMessage = stepResponse.errorMessage;

            return EMCPCommandStatus::FAILURE;
        }

        if( stepResponse.completed == false )
        {
            return EMCPCommandStatus::DEFERRED;
        }

        m_resumeFrame = stepResponse.resumeFrame;
        m_resumeTimestamp = stepResponse.resumeTimestamp;
        m_stepStartFrame = currentFrame;
        ++m_index;

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputSequenceOperation::cancel()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
