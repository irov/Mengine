#include "MCPScriptReleaseCommand.h"

#include "../../Contexts/MCPScriptContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptReleaseCommand::MCPScriptReleaseCommand( MCPScriptContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptReleaseCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object handlesObject;

        if( _request.params.exist( "handles", &handlesObject ) == false || handlesObject.is_type_array() == false )
        {
            _response->errorMessage = "handles must be an array";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::array handles( handlesObject );
        uint32_t released = m_context->releaseHandles( handles );
        size_t remainingValue = m_context->getHandleCount();
        uint32_t remaining = (uint32_t)remainingValue;
        _response->result.set( "released", released );
        _response->result.set( "remaining", remaining );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
