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
        void updateVerticesWM() const override;
        void updateVerticesColor() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM() const;

    protected:
        mutable mt::vec2f m_verticesLocal[4];
        mutable RenderVertex2D m_verticesWM[4];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeQuad> ShapeQuadPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * ShapeQuad::getVerticesWM() const
    {
        this->prepareVerticesWM();

        return m_verticesWM;
    }
    //////////////////////////////////////////////////////////////////////////
}