#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPRuntimeWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPRuntimeWaitCondition, MCPWaitConditionInterface> MCPRuntimeWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
