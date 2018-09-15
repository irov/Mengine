#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Surface> SurfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Shape
        : public Node
        , public BaseUpdation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();

    public:
        Shape();
        ~Shape() override;

    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        void invalidateVerticesLocal();
        void invalidateVerticesWM();
        void invalidateVerticesColor();

    protected:
        SurfacePtr m_surface;

        mutable bool m_invalidateVerticesLocal;
        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Shape> ShapePtr;
    //////////////////////////////////////////////////////////////////////////
}