#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Surface.h"
#include "Kernel/RenderVertex2D.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SpriteSlice9
        : public Node
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( SpriteSlice9 );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        SpriteSlice9();
        ~SpriteSlice9() override;

    public:
        void setSurfacePart( uint32_t _index, const SurfacePtr & _surface );
        const SurfacePtr & getSurfacePart( uint32_t _index ) const;
        void removeSurfacePart( uint32_t _index );

        void setPartOffset( uint32_t _index, const mt::vec2f & _offset );
        const mt::vec2f & getPartOffset( uint32_t _index ) const;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;
        void removeSize();
        bool hasSize() const;

        mt::vec2f getMinimumSize() const;
        mt::vec2f getNaturalSize() const;

    public:
        const mt::box2f * getBoundingBox() const override;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void _invalidateColor() const override;
        void _invalidateWorldMatrix() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        void invalidateVerticesLocal() const;
        void invalidateVerticesWM() const;
        void invalidateVerticesColor() const;

    protected:
        void synchronizeSurfaceRevision() const;
        void prepareVerticesWM() const;

        void updateVerticesLocal() const;
        void updateVerticesWM() const;
        void updateVerticesColor() const;

        void calcBandSizes( float * const _bandX, float * const _bandY ) const;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM() const;

    protected:
        SurfacePtr m_surfaces[9];
        mt::vec2f m_partOffsets[9];

        mt::vec2f m_size;

        mutable uint32_t m_surfaceRevisions[9];

        mutable mt::vec2f m_verticesLocal[36];
        mutable RenderVertex2D m_verticesWM[36];

        mutable bool m_invalidateVerticesLocal;
        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<SpriteSlice9> SpriteSlice9Ptr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * SpriteSlice9::getVerticesWM() const
    {
        this->prepareVerticesWM();

        return m_verticesWM;
    }
    //////////////////////////////////////////////////////////////////////////
}
