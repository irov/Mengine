#pragma once

#include "Kernel/RenderVisitor.h"
#include "Kernel/Factorable.h"

#include "Kernel/Node.h"

#include "Kernel/Polygon.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class DebugRenderVisitor> DebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
    class DebugRenderVisitor
        : public RenderVisitor
        , public Factorable
        , public ConcreteVisitor<Node>
    {
    public:
        DebugRenderVisitor();
        ~DebugRenderVisitor() override;

    public:
        void addDebugRenderVisitor( const DebugRenderVisitorPtr & _visitor );
        void removeDebugRenderVisitor( const DebugRenderVisitorPtr & _visitor );

    protected:
        void accept( Node * _node ) override;

    protected:
        void renderLine_( RenderVertex2D * _vertices, uint32_t _count );
        void renderCircle_( const mt::mat4f & _wm, float _radius, uint32_t _count, uint32_t _color );
        void renderPolygon_( const mt::mat4f & _wm, const Polygon & _polygon, uint32_t _color );

    protected:
        typedef Vector<DebugRenderVisitorPtr> VectorDebugRenderVisitor;
        VectorDebugRenderVisitor m_debugRenderVisitors;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DebugRenderVisitor> DebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}