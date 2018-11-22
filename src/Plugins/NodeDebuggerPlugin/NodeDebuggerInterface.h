#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/Node.h"
#include "Kernel/RenderVisitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Scene, class Node> ScenePtr;

    //////////////////////////////////////////////////////////////////////////
    class NodeDebuggerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NodeDebuggerService" );

    public:
        virtual void setScene( const Mengine::ScenePtr & _scene ) = 0;
        virtual void update() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define NODEDEBUGGER_SERVICE()\
    ((Mengine::NodeDebuggerServiceInterface *)SERVICE_GET(Mengine::NodeDebuggerServiceInterface))
}