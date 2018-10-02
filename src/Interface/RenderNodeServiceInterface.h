#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    class RenderNodeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderNodeService" )

    public:
        virtual void invalidateNodeCache() = 0;
        virtual void renderNode( const RenderContext * _context, const RenderInterfacePtr & _render ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERNODE_SERVICE()\
    ((Mengine::RenderNodeServiceInterface*)SERVICE_GET(Mengine::RenderNodeServiceInterface))
}
