#include "MCPAppStopCommand.h"

#include "Interface/ApplicationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPAppStopCommand::MCPAppStopCommand( MCPCommandHostInterface * _host )
        : m_host( _host )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPAppStopCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        _response->result.set( "stopping", true );
        m_host->setMCPDebuggerPaused( false );
        APPLICATION_SERVICE()->quit();

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPAppStopCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
