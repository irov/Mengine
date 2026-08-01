#include "MCPDebugPauseCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugPauseCommand::MCPDebugPauseCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugPauseCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        m_context->requestPause();
        _response->result.set( "requested", true );
        _response->result.set( "cooperative", true );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugPauseCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
