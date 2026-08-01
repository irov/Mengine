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
        MCPRuntimeControlCommand( MCPCommandHostInterface * _host, MCPRuntimeContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        void update() override;
        void cancel( uint32_t _requestId ) override;
        void disconnect() override;

    protected:
        typedef EMCPCommandStatus (MCPRuntimeControlCommand::*Action)( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        typedef Map<ConstString, Action> MapActions;

        EMCPCommandStatus pause_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus resume_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus timeScale_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );
        EMCPCommandStatus advanceFrames_( const MCPCommandRequest & _request, MCPCommandResponse * const _response );

        void writeState_( const Char * _action, MCPCommandResponse * const _response ) const;
        void restoreAdvance_();

    protected:
        struct PendingAdvance
        {
            uint32_t requestId = 0;
            uint64_t startGeneration = 0;
            uint32_t frames = 0;
            float restoreTimeFactorBase = 1.f;
            int32_t restoreTimeFactorCount = 0;
        };

        MCPCommandHostInterface * m_host;
        MCPRuntimeContext * m_context;
        MapActions m_actions;
        PendingAdvance m_pendingAdvance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPRuntimeControlCommand, MCPCommandInterface> MCPRuntimeControlCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
