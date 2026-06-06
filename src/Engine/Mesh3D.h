#pragma once

#include "ResourceMesh3D.h"
#include "Interface/Lighting3DServiceInterface.h"

#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderProgramVariableInterface.h"
#include "Interface/RenderTransformationInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ConstString.h"
#include "Kernel/Vector.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class Mesh3D
        : public Node
        , public RenderTransformationInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Mesh3D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Mesh3D();
        ~Mesh3D() override;

    public:
        void setResourceMesh3D( const ResourceMesh3DPtr & _resource );
        const ResourceMesh3DPtr & getResourceMesh3D() const;

        void setResourceImage( const ResourceImagePtr & _resource );
        const ResourceImagePtr & getResourceImage() const;

        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        ResourceMesh3DPtr m_resourceMesh;
        ResourceImagePtr m_resourceImage;
        ConstString m_materialName;

        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;
        RenderMaterialInterfacePtr m_material;

        RenderProgramVariableInterfacePtr m_programVariable;

        mutable Vector<float> m_uniformUpload;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Mesh3D> Mesh3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
