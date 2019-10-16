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
        void updateVerticesLocal() const override;
        void updateVerticesWM() const override;
        void updateVerticesColor() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const override;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM() const;

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
    typedef IntrusiveNodePtr<ShapePacMan> ShapePacManPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * ShapePacMan::getVerticesWM() const
    {
        this->prepareVerticesWM();

        return m_verticesWM;
    }
}