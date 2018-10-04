#pragma once

#include "Kernel/DebugRenderVisitor.h"

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
        : public DebugRenderVisitor
        , public ConcreteVisitor<HotSpotCircle>
        , public ConcreteVisitor<HotSpotPolygon>
        , public ConcreteVisitor<HotSpotImage>
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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebugRenderVisitor> NodeDebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}