#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPRuntimeContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPLogWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        explicit MCPLogWaitCondition( MCPRuntimeContext * _context );

        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;

    protected:
        MCPRuntimeContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPLogWaitCondition, MCPWaitConditionInterface> MCPLogWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
