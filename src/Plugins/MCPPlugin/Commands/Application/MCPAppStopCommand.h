#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPAppStopCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPAppStopCommand( MCPCommandHostInterface * _host );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        MCPCommandHostInterface * m_host;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPAppStopCommand, MCPCommandInterface> MCPAppStopCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
