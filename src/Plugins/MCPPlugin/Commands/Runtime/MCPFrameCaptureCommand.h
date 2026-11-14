#pragma once

#include "../MCPCommandInterface.h"

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
        ~MCPFrameCaptureCommand() override;

    public:
        void endCapture();

    public:
        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPCommandHostInterface * m_host;
        bool m_capturing;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPFrameCaptureCommand, MCPCommandInterface> MCPFrameCaptureCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
