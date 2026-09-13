#pragma once

#include "Kernel/Shape.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/RenderVertex2D.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeThreePatchHorizontal
        : public Shape
    {
        DECLARE_FACTORABLE( ShapeThreePatchHorizontal );
        DECLARE_VISITABLE( Node );

    public:
        ShapeThreePatchHorizontal();
        ~ShapeThreePatchHorizontal() override;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;
        void removeSize();
        bool hasSize() const;

        void setCapInsets( const mt::vec2f & _capInsets );
        const mt::vec2f & getCapInsets() const;
        mt::vec2f getMinimumSize() const;

    protected:
        void updateVerticesLocal() const override;
        void updateVerticesWM() const override;
        void updateVerticesColor() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM() const;

    protected:
        mt::vec2f m_size;
        mt::vec2f m_capInsets;

        mutable mt::vec2f m_verticesLocal[8];
        mutable RenderVertex2D m_verticesWM[8];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeThreePatchHorizontal> ShapeThreePatchHorizontalPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * ShapeThreePatchHorizontal::getVerticesWM() const
    {
        this->prepareVerticesWM();

        return m_verticesWM;
    }
    //////////////////////////////////////////////////////////////////////////
}
