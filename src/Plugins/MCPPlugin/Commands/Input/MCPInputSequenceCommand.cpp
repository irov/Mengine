#include "MCPInputSequenceCommand.h"

#include "MCPInputSequenceOperation.h"

#include "Kernel/FactorableUnique.h"

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
        jpp::object steps;
        if( _request.params.exist( "steps", &steps ) == false )
        {
            _response->errorMessage = "input_sequence steps must be a non-empty array";

            return EMCPCommandStatus::FAILURE;
        }

        if( steps.is_type_array() == false )
        {
            _response->errorMessage = "input_sequence steps must be a non-empty array";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::array stepsArray( steps );

        if( stepsArray.empty() == true )
        {
            _response->errorMessage = "input_sequence steps must be a non-empty array";

            return EMCPCommandStatus::FAILURE;
        }

        uint64_t startFrame = m_host->getMCPUpdateFrame();

        _response->operation = Helper::makeFactorableUnique<MCPInputSequenceOperation>( MENGINE_DOCUMENT_FACTORABLE, m_host, m_registry, steps, startFrame );

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
}
