#pragma once

#include "Kernel/RenderVisitor.h"
#include "Kernel/Factorable.h"

#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotImage.h"
#include "Engine/Point.h"

#include "Kernel/Arrow.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderVisitor
        : public RenderVisitor
        , public Factorable
        , public ConcreteVisitor<HotSpotCircle>
        , public ConcreteVisitor<HotSpotPolygon>
        , public ConcreteVisitor<HotSpotImage>
        //, public ConcreteVisitor<MovieSlot>
        , public ConcreteVisitor<Point>
        , public ConcreteVisitor<Arrow>
    {
    public:
        NodeDebugRenderVisitor();
        ~NodeDebugRenderVisitor() override;

    protected:
        void accept( HotSpotCircle * _node ) override;
        void accept( HotSpotPolygon * _node ) override;
        void accept( HotSpotImage * _node ) override;
        //void accept( MovieSlot * _node ) override;
        void accept( Point * _node ) override;
        void accept( Arrow * _node ) override;

    protected:
        void renderLine_( RenderVertex2D * _vertices, uint32_t _count );
        void renderCircle_( const mt::mat4f & _wm, float _radius, uint32_t _count, uint32_t _color );
        void renderPolygon_( const mt::mat4f & _wm, const Polygon & _polygon, uint32_t _color );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebugRenderVisitor> NodeDebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}