#include "MCPTouchInputSequenceStep.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPTouchInputSequenceStep::MCPTouchInputSequenceStep( const MCPCommandInterfacePtr & _command )
        : m_command( _command )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPTouchInputSequenceStep::execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response )
    {
        jpp::object params;

        if( _request.step.exist( "params", &params ) == false || params.is_type_object() == false )
        {
            params = _request.step;
        }

        Data attachment;
        MCPCommandRequest commandRequest = {0, params, attachment};
        MCPCommandResponse commandResponse;
        EMCPCommandStatus status = m_command->execute( commandRequest, &commandResponse );

        if( status != EMCPCommandStatus::SUCCESS )
        {
            _response->errorCode = commandResponse.errorCode;
            _response->errorMessage = commandResponse.errorMessage;

            return false;
        }

        _response->completed = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
