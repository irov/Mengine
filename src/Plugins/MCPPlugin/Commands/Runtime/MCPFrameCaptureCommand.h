#pragma once

#include "../MCPCommandInterface.h"

#include "Interface/RenderTargetInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPFrameCaptureCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPFrameCaptureCommand( MCPCommandHostInterface * _host );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        void update() override;
        void cancel( uint32_t _requestId ) override;
        void disconnect() override;

    protected:
        bool encodeFrame_( Data * const _png, String * const _error );
        void reset_();

    protected:
        struct PendingCapture
        {
            uint32_t requestId = 0;
            uint32_t width = 0;
            uint32_t height = 0;
            uint64_t armedGeneration = 0;
            bool includeAlpha = false;
            RenderTargetInterfacePtr target;
            RenderTargetInterfacePtr previousTarget;
        };

        MCPCommandHostInterface * m_host;
        PendingCapture m_pendingCapture;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPFrameCaptureCommand, MCPCommandInterface> MCPFrameCaptureCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
