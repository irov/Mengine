#include "MCPDebugContinueCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugContinueCommand::MCPDebugContinueCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugContinueCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        bool continued = m_context->continueExecution();

        if( continued == false )
        {
            _response->errorMessage = "runtime is not paused";

            return EMCPCommandStatus::FAILURE;
        }

        _response->result.set( "continued", true );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugContinueCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
