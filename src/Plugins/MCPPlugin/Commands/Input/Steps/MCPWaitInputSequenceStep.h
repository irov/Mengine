#pragma once

#include "MCPInputSequenceStepInterface.h"
#include "../../Wait/MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPWaitInputSequenceStep
        : public MCPInputSequenceStepInterface
        , public Factorable
    {
    public:
        explicit MCPWaitInputSequenceStep( MCPWaitConditionRegistry * _registry );

        bool execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response ) override;

    protected:
        MCPWaitConditionRegistry * m_registry;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPWaitInputSequenceStep, MCPInputSequenceStepInterface> MCPWaitInputSequenceStepPtr;
    //////////////////////////////////////////////////////////////////////////
}
