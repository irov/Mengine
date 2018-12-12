#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Scene.h"
#include "Kernel/RenderVisitor.h"

namespace Mengine
{
    class NodeDebuggerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NodeDebuggerService" );

    public:
        virtual void setScene( const ScenePtr & _scene ) = 0;
        virtual void update() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define NODEDEBUGGER_SERVICE()\
    ((Mengine::NodeDebuggerServiceInterface *)SERVICE_GET(Mengine::NodeDebuggerServiceInterface))
//////////////////////////////////////////////////////////////////////////
