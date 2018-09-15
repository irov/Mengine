#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderNodeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderNodeService" )

    public:
        virtual void invalidateNodeCache() = 0;
        virtual void renderNode( const RenderContext * _context, const NodePtr & _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERNODE_SERVICE()\
    ((Mengine::RenderNodeServiceInterface*)SERVICE_GET(Mengine::RenderNodeServiceInterface))
}
