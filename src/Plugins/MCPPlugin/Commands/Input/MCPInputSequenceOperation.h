#pragma once

#include "../MCPCommandInterface.h"
#include "Steps/MCPInputSequenceStepInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPInputSequenceOperation
        : public MCPOperationInterface
        , public Factorable
    {
    public:
        MCPInputSequenceOperation( MCPCommandHostInterface * _host, MCPInputSequenceStepRegistry * _registry, const jpp::object & _steps, uint64_t _startFrame );
        ~MCPInputSequenceOperation() override;

    public:
        EMCPCommandStatus update( MCPCommandResponse * const _response ) override;
        void cancel() override;

    protected:
        MCPCommandHostInterface * m_host;
        MCPInputSequenceStepRegistry * m_registry;
        jpp::object m_steps;
        uint64_t m_startFrame;
        uint64_t m_stepStartFrame;
        uint64_t m_resumeFrame;
        Timestamp m_resumeTimestamp;
        size_t m_index;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputSequenceOperation, MCPOperationInterface> MCPInputSequenceOperationPtr;
    //////////////////////////////////////////////////////////////////////////
}
