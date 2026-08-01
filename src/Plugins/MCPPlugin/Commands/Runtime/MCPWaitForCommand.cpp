#include "MCPWaitForCommand.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPWaitForCommand::MCPWaitForCommand( MCPCommandHostInterface * _host, MCPWaitConditionRegistry * _registry )
        : m_host( _host )
        , m_registry( _registry )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPWaitForCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        if( m_pendingWait.requestId != 0 )
        {
            _response->errorMessage = "another wait_for operation is already pending";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object conditionParams;

        if( _request.params.exist( "condition", &conditionParams ) == false || conditionParams.is_type_object() == false )
        {
            _response->errorMessage = "wait_for condition must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * typeValue = conditionParams.get( "type", "" );
        ConstString typeName = Helper::stringizeString( typeValue );
        MCPWaitConditionInterfacePtr condition = m_registry->findCondition( typeName );

        if( condition == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "unsupported wait_for condition type";

            return EMCPCommandStatus::FAILURE;
        }

        m_pendingWait.requestId = _request.requestId;
        m_pendingWait.startGeneration = m_host->getMCPUpdateGeneration();
        m_pendingWait.condition = condition;
        m_pendingWait.params = conditionParams;

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPWaitForCommand::update()
    {
        if( m_pendingWait.requestId == 0 )
        {
            return;
        }

        if( m_host->isMCPConnected() == false )
        {
            m_pendingWait = PendingWait();

            return;
        }

        uint32_t requestId = m_pendingWait.requestId;
        uint64_t currentGeneration = m_host->getMCPUpdateGeneration();
        MCPWaitConditionRequest conditionRequest = {m_pendingWait.params, m_pendingWait.startGeneration, currentGeneration};
        MCPWaitConditionResponse conditionResponse;
        bool evaluated = m_pendingWait.condition->evaluate( conditionRequest, &conditionResponse );

        if( evaluated == false )
        {
            const Char * errorCode = conditionResponse.errorCode.empty() == true ? "execution_error" : conditionResponse.errorCode.c_str();
            const Char * errorMessage = conditionResponse.errorMessage.c_str();
            m_host->sendMCPCommandError( requestId, errorCode, errorMessage );
            m_pendingWait = PendingWait();

            return;
        }

        if( conditionResponse.satisfied == false )
        {
            return;
        }

        uint64_t framesElapsed = currentGeneration - m_pendingWait.startGeneration;
        jpp::object result = jpp::make_object();
        result.set( "satisfied", true );
        result.set( "details", conditionResponse.details );
        result.set( "framesElapsed", (int64_t)framesElapsed );
        m_host->sendMCPCommandResponse( requestId, result );
        m_pendingWait = PendingWait();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPWaitForCommand::cancel( uint32_t _requestId )
    {
        if( m_pendingWait.requestId != _requestId )
        {
            return;
        }

        m_pendingWait = PendingWait();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPWaitForCommand::disconnect()
    {
        m_pendingWait = PendingWait();
    }
    //////////////////////////////////////////////////////////////////////////
}
