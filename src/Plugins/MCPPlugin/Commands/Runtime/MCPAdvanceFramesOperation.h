#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class MCPRuntimeContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPAdvanceFramesOperation
        : public MCPOperationInterface
        , public Factorable
        , public Observable
    {
    public:
        MCPAdvanceFramesOperation( MCPRuntimeContext * _context, uint32_t _frames, float _timeFactor );
        ~MCPAdvanceFramesOperation() override;

    public:
        void begin();

    public:
        EMCPCommandStatus update( MCPCommandResponse * const _response ) override;
        void cancel() override;

    protected:
        void end_();
        void notifyApplicationBeginUpdate_();

    protected:
        MCPRuntimeContext * m_context;
        uint32_t m_frames;
        uint32_t m_framesAdvanced;
        float m_timeFactor;
        float m_previousTimeFactorBase;
        int32_t m_previousTimeFactorCount;
        bool m_advancing;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPAdvanceFramesOperation, MCPOperationInterface> MCPAdvanceFramesOperationPtr;
    //////////////////////////////////////////////////////////////////////////
}
