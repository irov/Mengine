#pragma once

#include "Interface/RenderTargetInterface.h"

#include "MCPFrameCaptureCommand.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPFrameCaptureOperation
        : public MCPOperationInterface
        , public Factorable
    {
    public:
        MCPFrameCaptureOperation( MCPFrameCaptureCommand * _command, MCPCommandHostInterface * _host, const RenderTargetInterfacePtr & _target, bool _ownTarget, uint32_t _width, uint32_t _height, bool _includeAlpha );
        ~MCPFrameCaptureOperation() override;

    public:
        EMCPCommandStatus update( MCPCommandResponse * const _response ) override;
        void cancel() override;

    protected:
        bool encodeFrame_( Data * const _png, String * const _error ) const;
        void restoreTarget_();

    protected:
        MCPFrameCaptureCommandPtr m_command;
        MCPCommandHostInterface * m_host;
        RenderTargetInterfacePtr m_target;
        bool m_ownTarget;
        uint32_t m_width;
        uint32_t m_height;
        bool m_includeAlpha;
        uint64_t m_armedFrame;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPFrameCaptureOperation, MCPOperationInterface> MCPFrameCaptureOperationPtr;
    //////////////////////////////////////////////////////////////////////////
}
