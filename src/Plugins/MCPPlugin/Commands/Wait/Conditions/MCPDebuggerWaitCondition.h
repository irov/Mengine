#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPCommandHostInterface;

    //////////////////////////////////////////////////////////////////////////
    class MCPDebuggerWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        explicit MCPDebuggerWaitCondition( MCPCommandHostInterface * _host );

        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;

    protected:
        MCPCommandHostInterface * m_host;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDebuggerWaitCondition, MCPWaitConditionInterface> MCPDebuggerWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
