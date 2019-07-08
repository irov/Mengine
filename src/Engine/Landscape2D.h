#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorResourceImages.h"

#include "Config/Vector.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderMaterial;
    struct RenderMaterialGroup;
    //////////////////////////////////////////////////////////////////////////
    struct Landscape2DElement
    {
        ResourceImagePtr image;
        mutable RenderMaterialInterfacePtr material;
        uint32_t i;
        uint32_t j;
        mt::box2f bb;
        mutable mt::box2f bb_wm;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<Landscape2DElement> VectorLandscape2DElements;
    //////////////////////////////////////////////////////////////////////////
    class Landscape2D
        : public Node
        , public BaseRender
        , public Materialable
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        Landscape2D();
        ~Landscape2D() override;

    public:
        void setBackParts( const VectorResourceImages & _images, uint32_t _countX, uint32_t _countY, float _width, float _height );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        void _invalidateWorldMatrix() override;

    protected:
        void updateVerticesWM_() const;
        void updateElementWM_() const;

    protected:
        MENGINE_INLINE const RenderVertex2D * getVerticesWM( uint32_t _offset ) const;
        MENGINE_INLINE const VectorLandscape2DElements & getElementWM() const;

    protected:
        bool compileResources_();

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        VectorResourceImages m_images;

        uint32_t m_elementCountX;
        uint32_t m_elementCountY;
        float m_elementWidth;
        float m_elementHeight;

        VectorLandscape2DElements m_elements;

        mutable VectorRenderVertex2D m_verticesWM;

        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateElementWM;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderVertex2D * Landscape2D::getVerticesWM( uint32_t _offset ) const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_();
        }

        return &m_verticesWM[_offset];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorLandscape2DElements & Landscape2D::getElementWM() const
    {
        if( m_invalidateElementWM == true )
        {
            this->updateElementWM_();
        }

        return m_elements;
    }
}
