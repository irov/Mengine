#include "MCPSceneGetCommand.h"

#include "../../Contexts/MCPSceneContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPSceneGetCommand::MCPSceneGetCommand( MCPSceneContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPSceneGetCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        m_context->refreshGeneration();

        const Char * handle = _request.params.get( "handle", "" );
        String error;
        NodePtr node = m_context->resolveNode( handle, &error );

        if( node == nullptr )
        {
            _response->errorCode = "stale_handle";
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object result = m_context->makeNodePropertiesJSON( node );
        _response->result = result;

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
