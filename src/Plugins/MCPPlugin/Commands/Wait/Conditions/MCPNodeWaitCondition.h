#pragma once

#include "../MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPNodeWaitCondition
        : public MCPWaitConditionInterface
        , public Factorable
    {
    public:
        explicit MCPNodeWaitCondition( MCPSceneContext * _context );

        bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) override;

    protected:
        MCPSceneContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPNodeWaitCondition, MCPWaitConditionInterface> MCPNodeWaitConditionPtr;
    //////////////////////////////////////////////////////////////////////////
}
