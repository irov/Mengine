#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<class Point> PointPtr;
    //////////////////////////////////////////////////////////////////////////
    class Point
        : public Node
        , public BaseRender
        , public Materialable
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        Point();
        ~Point() override;

    public:
        void setLinkedPoint( const PointPtr & _linked );
        void removeLinkedPoint();
        const PointPtr & getLinkedPoint() const;

    public:
        void setWidth( float _width );
        float getWidth() const;

    protected:
        void _dispose() override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        PointPtr m_linked;
        bool m_owner;

        float m_width;

        mutable RenderVertex2D m_vertices[4];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Point> PointPtr;
    //////////////////////////////////////////////////////////////////////////
}
