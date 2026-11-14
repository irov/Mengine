#include "MCPWaitForOperation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPWaitForOperation::MCPWaitForOperation( MCPCommandHostInterface * _host, const MCPWaitConditionInterfacePtr & _condition, const jpp::object & _params, uint64_t _startFrame )
        : m_host( _host )
        , m_condition( _condition )
        , m_params( _params )
        , m_startFrame( _startFrame )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPWaitForOperation::~MCPWaitForOperation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPWaitForOperation::update( MCPCommandResponse * const _response )
    {
        uint64_t currentFrame = m_host->getMCPUpdateFrame();

        MCPWaitConditionRequest conditionRequest = {m_params, m_startFrame, currentFrame};
        MCPWaitConditionResponse conditionResponse;
        if( m_condition->evaluate( conditionRequest, &conditionResponse ) == false )
        {
            _response->errorCode = conditionResponse.errorCode;
            _response->errorMessage = conditionResponse.errorMessage;

            return EMCPCommandStatus::FAILURE;
        }

        if( conditionResponse.satisfied == false )
        {
            return EMCPCommandStatus::DEFERRED;
        }

        uint64_t framesElapsed = currentFrame - m_startFrame;

        _response->result.set( "satisfied", true );
        _response->result.set( "details", conditionResponse.details );
        _response->result.set( "framesElapsed", (int64_t)framesElapsed );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPWaitForOperation::cancel()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
