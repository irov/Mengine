#pragma once

#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderTransformationInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ConstString.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class BlobShadow3D
        : public Node
        , public RenderTransformationInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( BlobShadow3D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        BlobShadow3D();
        ~BlobShadow3D() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resource );
        const ResourceImagePtr & getResourceImage() const;

        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

    public:
        void setShadowSize( float _width, float _depth );
        float getShadowWidth() const;
        float getShadowDepth() const;

        void setShadowColor( float _r, float _g, float _b, float _a );

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void rebuildQuad_();

    protected:
        ResourceImagePtr m_resourceImage;
        ConstString m_materialName;

        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;
        RenderMaterialInterfacePtr m_material;

        float m_width;
        float m_depth;

        uint32_t m_color;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<BlobShadow3D> BlobShadow3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
