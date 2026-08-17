#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Materialable.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Grid2D
        : public Node
        , public Materialable
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Grid2D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Grid2D();
        ~Grid2D() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;

    public:
        void setOffset( const mt::vec2f & _offset );
        const mt::vec2f & getOffset() const;

    public:
        void setWidth( float _width );
        float getWidth() const;

        void setHeight( float _height );
        float getHeight() const;

        void setCountX( uint32_t _count );
        uint32_t getCountX() const;

        void setCountY( uint32_t _count );
        uint32_t getCountY() const;

        bool setGridColor( uint32_t _i, uint32_t _j, const Color & _value );
        bool getGridColor( uint32_t _i, uint32_t _j, Color * const _value ) const;

        bool setGridPosition( uint32_t _i, uint32_t _j, const mt::vec3f & _value );
        bool getGridPosition( uint32_t _i, uint32_t _j, mt::vec3f * const _value ) const;
        bool getOriginalGridPosition( uint32_t _i, uint32_t _j, mt::vec3f * const _value ) const;
        void resetGrid();

        void applyWaves( float _phase, float _amplitude, float _waves, bool _horizontal, bool _vertical );
        void applyRipple( const mt::vec2f & _center, float _radius, float _amplitude, float _waves, float _phase );
        void applyLiquid( float _phase, float _amplitude, float _waves );
        void applyShaky( float _range, uint32_t _seed );
        void applyShuffle( float _progress, uint32_t _seed );
        void applySplit( float _progress, float _distance, bool _horizontal );
        void applyPageTurn( float _progress, float _radius );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _dispose() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _invalidateWorldMatrix() const override;

        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        void updateVerticesWM_() const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM() const;

    protected:
        ResourceImagePtr m_resourceImage;

        float m_width;
        float m_height;

        float m_angle;
        mt::vec2f m_offset;

        uint32_t m_countX;
        uint32_t m_countY;

        VectorRenderIndex m_indices;

        VectorRenderVertex2D m_vertices;
        VectorRenderVertex2D m_originalVertices;
        mutable VectorRenderVertex2D m_verticesWM;

        mutable bool m_invalidateVerticesWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Grid2D> Grid2DPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * Grid2D::getVerticesWM() const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_();
        }

        return m_verticesWM.data();
    }
    //////////////////////////////////////////////////////////////////////////
}
