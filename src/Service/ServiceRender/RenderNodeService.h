#pragma once

#include "Interface/RenderNodeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderNodeService
        : public ServiceBase<RenderNodeServiceInterface>
    {
    public:
        RenderNodeService();
        ~RenderNodeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void _stopService() override;

    public:
        void invalidateNodeCache() override;
        void renderNode( const RenderContext * _context, const NodePtr & _node ) override;

    protected:
        void cacheNode_( const RenderContext * _context, const NodePtr & _node );

    protected:
        struct RenderNodeDesc
        {
            RenderInterfacePtr render;

            RenderContext context;
        };

        typedef Vector<RenderNodeDesc> VectorRenderNodies;
        VectorRenderNodies m_nodies;

        bool m_invalidate;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERNODE_SERVICE()\
    ((Mengine::RenderNodeServiceInterface*)SERVICE_GET(Mengine::RenderNodeServiceInterface))
}
