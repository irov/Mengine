#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class MCPRuntimeContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPRuntimeControlCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPRuntimeControlCommand( MCPRuntimeContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        typedef EMCPCommandStatus (MCPRuntimeControlCommand::*Action)( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        typedef Map<ConstString, Action> MapActions;

        EMCPCommandStatus pause_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus resume_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus timeScale_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus advanceFrames_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );

        void writeState_( const Char * _action, MCPCommandResponse * const _response ) const;

    protected:
        MCPRuntimeContext * m_context;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPRuntimeControlCommand, MCPCommandInterface> MCPRuntimeControlCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
