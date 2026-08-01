#pragma once

#include "MCPInputSequenceStepInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPDelayInputSequenceStep
        : public MCPInputSequenceStepInterface
        , public Factorable
    {
    public:
        bool execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDelayInputSequenceStep, MCPInputSequenceStepInterface> MCPDelayInputSequenceStepPtr;
    //////////////////////////////////////////////////////////////////////////
}
