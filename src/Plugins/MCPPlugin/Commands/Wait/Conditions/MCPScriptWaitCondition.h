#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        explicit MCPScriptWaitCondition( MCPScriptContext * _scriptContext );

        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;

    protected:
        MCPScriptContext * m_scriptContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptWaitCondition, MCPWaitConditionInterface> MCPScriptWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
