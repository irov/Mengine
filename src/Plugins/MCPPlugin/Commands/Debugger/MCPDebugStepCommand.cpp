#include "MCPDebugStepCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugStepCommand::MCPDebugStepCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugStepCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * kindValue = _request.params.get( "kind", "" );
        String kind = kindValue;
        bool supported = kind == "in" || kind == "over" || kind == "out";

        if( supported == false )
        {
            _response->errorMessage = "unknown step kind";

            return EMCPCommandStatus::FAILURE;
        }

        bool stepping = m_context->startStep( kind );

        if( stepping == false )
        {
            _response->errorMessage = "runtime is not paused";

            return EMCPCommandStatus::FAILURE;
        }

        _response->result.set( "stepping", kind );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugStepCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
