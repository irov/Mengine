#pragma once

#include "Kernel/Shape.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeQuad
        : public Shape
    {
    public:
        ShapeQuad();
        ~ShapeQuad() override;
		
    protected:
        virtual void updateVerticesLocal() const = 0;

    protected:
        virtual void updateVerticesWM() const;
        virtual void updateVerticesColor() const;

    protected:
        void _render( const RenderContext * _state ) override;

    protected:
        void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

    protected:
        inline const RenderVertex2D * getVerticesWM() const;

	protected:
        mutable mt::vec2f m_verticesLocal[4];
        mutable RenderVertex2D m_verticesWM[4];
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * ShapeQuad::getVerticesWM() const
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

        return m_verticesWM;
    }
}