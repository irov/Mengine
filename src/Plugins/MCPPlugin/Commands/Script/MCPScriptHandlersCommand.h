#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPHandlerRegistry;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptHandlersCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPScriptHandlersCommand( const MCPHandlerRegistry * _registry );
        ~MCPScriptHandlersCommand() override;

    public:
        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        const MCPHandlerRegistry * m_registry;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptHandlersCommand, MCPCommandInterface> MCPScriptHandlersCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
