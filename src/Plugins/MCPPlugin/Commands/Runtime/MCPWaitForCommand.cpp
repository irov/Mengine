#include "MCPWaitForCommand.h"

#include "MCPWaitForOperation.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

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
        jpp::object conditionParams;
        if( _request.params.exist( "condition", &conditionParams ) == false )
        {
            _response->errorMessage = "wait_for condition must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        if( conditionParams.is_type_object() == false )
        {
            _response->errorMessage = "wait_for condition must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object typeObject;
        if( conditionParams.exist( "type", &typeObject ) == false )
        {
            _response->errorMessage = "wait_for condition type must be a string";

            return EMCPCommandStatus::FAILURE;
        }

        if( typeObject.is_type_string() == false )
        {
            _response->errorMessage = "wait_for condition type must be a string";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * typeValue = typeObject;
        ConstString typeName = Helper::stringizeString( typeValue );
        MCPWaitConditionInterfacePtr condition = m_registry->findCondition( typeName );

        if( condition == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "unsupported wait_for condition type";

            return EMCPCommandStatus::FAILURE;
        }

        uint64_t startFrame = m_host->getMCPUpdateFrame();

        _response->operation = Helper::makeFactorableUnique<MCPWaitForOperation>( MENGINE_DOCUMENT_FACTORABLE, m_host, condition, conditionParams, startFrame );

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
}
