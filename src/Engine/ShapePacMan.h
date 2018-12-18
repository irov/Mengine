#pragma once

#include "Kernel/Shape.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    class ShapePacMan
        : public Shape
    {
    public:
        ShapePacMan();
        ~ShapePacMan() override;

    public:
        void setAngleFrom( float _angleFrom );
        float getAngleFrom() const;

        void setAngleTo( float _angleTo );
        float getAngleTo() const;

    protected:
        void updateVerticesLocal() const;

    protected:
        void updateVerticesWM() const;
        void updateVerticesColor() const;

    protected:
        void _render( const RenderContext * _state ) override;

    protected:
        void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const override;

    protected:
        inline const RenderVertex2D * getVerticesWM() const;

    protected:
        float m_angleFrom;
        float m_angleTo;

        mutable RenderIndex m_indices[16];
        mutable mt::vec2f m_verticesLocal[8];
        mutable RenderVertex2D m_verticesWM[8];

        mutable uint32_t m_indexCount;
        mutable uint32_t m_vertexCount;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * ShapePacMan::getVerticesWM() const
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