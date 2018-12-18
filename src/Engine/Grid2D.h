#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceHolder.h"

#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
    class Grid2D
        : public Node
        , public BaseRender
        , public Materialable
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        Grid2D();
        ~Grid2D() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;

    public:
        void setAngle( const mt::vec2f & _offset, float _angle );

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
        bool getGridColor( uint32_t _i, uint32_t _j, Color & _value ) const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _render( const RenderContext * _state ) override;

    protected:
        void _invalidateWorldMatrix() override;

        void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const override;

    protected:
        void updateVerticesWM_();

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        inline const RenderVertex2D * getVerticesWM();

    protected:
        ResourceHolder<ResourceImage> m_resourceImage;

        float m_width;
        float m_height;

        float m_angle;
        mt::vec2f m_offset;

        uint32_t m_countX;
        uint32_t m_countY;

        VectorRenderVertex2D m_vertices;
        VectorRenderVertex2D m_verticesWM;

        VectorRenderIndices m_indices;

        mutable bool m_invalidateVerticesWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Grid2D> Grid2DPtr;
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * Grid2D::getVerticesWM()
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_();
        }

        return &m_verticesWM[0];
    }
}
