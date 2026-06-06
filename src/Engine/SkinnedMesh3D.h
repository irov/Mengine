#pragma once

#include "ResourceSkinnedMesh3D.h"
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

#ifndef MENGINE_SKINNED_MESH3D_PALETTE_MAX
#define MENGINE_SKINNED_MESH3D_PALETTE_MAX 64
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class SkinnedMesh3D
        : public Node
        , public RenderTransformationInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( SkinnedMesh3D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        SkinnedMesh3D();
        ~SkinnedMesh3D() override;

    public:
        void setResourceSkinnedMesh3D( const ResourceSkinnedMesh3DPtr & _resource );
        const ResourceSkinnedMesh3DPtr & getResourceSkinnedMesh3D() const;

        void setResourceImage( const ResourceImagePtr & _resource );
        const ResourceImagePtr & getResourceImage() const;

        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

    public:
        void setBoneMatrices( const mt::mat4f * _matrices, uint32_t _count );

        uint32_t getBoneMatrixCount() const;
        const mt::mat4f * getBoneMatrices() const;

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void uploadBonePalette_() const;

    protected:
        ResourceSkinnedMesh3DPtr m_resource;
        ResourceImagePtr m_resourceImage;
        ConstString m_materialName;

        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;
        RenderMaterialInterfacePtr m_material;
        RenderProgramVariableInterfacePtr m_programVariable;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;

        Vector<mt::mat4f> m_boneMatrices;

        mutable Vector<float> m_paletteUpload;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<SkinnedMesh3D> SkinnedMesh3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
