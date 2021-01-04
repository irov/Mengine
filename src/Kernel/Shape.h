#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Shape
        : public Node
        , private BaseUpdation
        , private BaseRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();

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
        void _dispose() override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        void _invalidateColor() const override;
        void _invalidateWorldMatrix() const override;

    protected:
        void invalidateVerticesLocal() const;
        void invalidateVerticesWM() const;
        void invalidateVerticesColor() const;

    protected:
        virtual void updateVerticesLocal() const = 0;
        virtual void updateVerticesWM() const = 0;
        virtual void updateVerticesColor() const = 0;

    protected:
        MENGINE_INLINE void prepareVerticesWM() const;

    protected:
        SurfacePtr m_surface;

        uint32_t m_surfaceRevision;

        mutable bool m_invalidateVerticesLocal;
        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Shape> ShapePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void Shape::prepareVerticesWM() const
    {
        if( m_invalidateVerticesLocal == true )
        {
            m_invalidateVerticesLocal = false;

            this->updateVerticesLocal();
        }

        if( m_invalidateVerticesWM == true )
        {
            m_invalidateVerticesWM = false;

            this->updateVerticesWM();
        }

        if( m_invalidateVerticesColor == true )
        {
            m_invalidateVerticesColor = false;

            this->updateVerticesColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}