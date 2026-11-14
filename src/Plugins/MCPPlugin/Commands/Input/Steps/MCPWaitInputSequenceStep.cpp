#include "MCPWaitInputSequenceStep.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPWaitInputSequenceStep::MCPWaitInputSequenceStep( MCPWaitConditionRegistry * _registry )
        : m_registry( _registry )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPWaitInputSequenceStep::execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response )
    {
        jpp::object conditionParams;
        if( _request.step.exist( "condition", &conditionParams ) == false )
        {
            _response->errorMessage = "input sequence wait step requires a condition";

            return false;
        }

        if( conditionParams.is_type_object() == false )
        {
            _response->errorMessage = "input sequence wait step requires a condition";

            return false;
        }

        jpp::object typeObject;
        if( conditionParams.exist( "type", &typeObject ) == false )
        {
            _response->errorMessage = "input sequence wait condition type must be a string";

            return false;
        }

        if( typeObject.is_type_string() == false )
        {
            _response->errorMessage = "input sequence wait condition type must be a string";

            return false;
        }

        const Char * typeValue = typeObject;
        ConstString typeName = Helper::stringizeString( typeValue );
        MCPWaitConditionInterfacePtr condition = m_registry->findCondition( typeName );

        if( condition == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "unsupported wait_for condition type";

            return false;
        }

        MCPWaitConditionRequest conditionRequest = {conditionParams, _request.startFrame, _request.currentFrame};
        MCPWaitConditionResponse conditionResponse;
        bool evaluated = condition->evaluate( conditionRequest, &conditionResponse );

        if( evaluated == false )
        {
            _response->errorCode = conditionResponse.errorCode;
            _response->errorMessage = conditionResponse.errorMessage;

            return false;
        }

        _response->completed = conditionResponse.satisfied;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
