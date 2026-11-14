#pragma once

#include "../MCPCommandInterface.h"
#include "../Wait/MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPWaitForOperation
        : public MCPOperationInterface
        , public Factorable
    {
    public:
        MCPWaitForOperation( MCPCommandHostInterface * _host, const MCPWaitConditionInterfacePtr & _condition, const jpp::object & _params, uint64_t _startFrame );
        ~MCPWaitForOperation() override;

    public:
        EMCPCommandStatus update( MCPCommandResponse * const _response ) override;
        void cancel() override;

    protected:
        MCPCommandHostInterface * m_host;
        MCPWaitConditionInterfacePtr m_condition;
        jpp::object m_params;
        uint64_t m_startFrame;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPWaitForOperation, MCPOperationInterface> MCPWaitForOperationPtr;
    //////////////////////////////////////////////////////////////////////////
}
