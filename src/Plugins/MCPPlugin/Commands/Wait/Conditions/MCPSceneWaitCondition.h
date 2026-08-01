#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPSceneWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPSceneWaitCondition, MCPWaitConditionInterface> MCPSceneWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
