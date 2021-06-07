#pragma once

#include "Kernel/Shape.h"
#include "Kernel/BaseRender.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeCircle
        : public Shape
    {
        DECLARE_FACTORABLE( ShapeCircle );
        DECLARE_VISITABLE( Node );

    public:
        ShapeCircle();
        ~ShapeCircle() override;

    public:
        void setQuality( uint8_t _quality );
        uint8_t getQuality() const;

    protected:
        void updateVerticesLocal() const override;
        void updateVerticesWM() const override;
        void updateVerticesColor() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f * _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM( uint32_t * const _count ) const;

    protected:
        uint8_t m_quality;

        mutable VectorRenderIndex m_indices;

        typedef Vector<mt::vec2f> VectorPositions;
        mutable VectorPositions m_positions;

        mutable VectorRenderVertex2D m_verticesWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeCircle> ShapeCirclePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * ShapeCircle::getVerticesWM( uint32_t * const _count ) const
    {
        this->prepareVerticesWM();

        const RenderVertex2D * vertices = m_verticesWM.data();

        *_count = (uint32_t)m_verticesWM.size();

        return vertices;
    }
    //////////////////////////////////////////////////////////////////////////
}
