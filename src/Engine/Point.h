#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
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
        void setLinkedPoint( Point * _linked );
        void removeLinkedPoint();
        Point * getLinkedPoint() const;

    public:
        void setWidth( float _width );
        float getWidth() const;

    protected:
        void _destroy() override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        Point * m_linked;
        bool m_owner;

        float m_width;

        mutable RenderVertex2D m_vertices[4];
    };
}
