#include "MCPDebugSetExceptionPolicyCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugSetExceptionPolicyCommand::MCPDebugSetExceptionPolicyCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugSetExceptionPolicyCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * policyValue = _request.params.get( "policy", "none" );
        String policy = policyValue;
        bool supported = policy == "none" || policy == "uncaught" || policy == "all";

        if( supported == false )
        {
            _response->errorMessage = "unknown exception policy";

            return EMCPCommandStatus::FAILURE;
        }

        m_context->setExceptionPolicy( policy );
        _response->result.set( "policy", policy );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugSetExceptionPolicyCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
