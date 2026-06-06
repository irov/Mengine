#include "SkinnedMesh3D.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        static const uint32_t SKINNED_MESH3D_PALETTE_REG_START = 4;
        static const uint32_t SKINNED_MESH3D_FLOATS_PER_BONE = 12;

        static const uint32_t SKINNED_MESH3D_LIGHTING_REG_START =
            SKINNED_MESH3D_PALETTE_REG_START + MENGINE_SKINNED_MESH3D_PALETTE_MAX * 3;
    }
    //////////////////////////////////////////////////////////////////////////
    SkinnedMesh3D::SkinnedMesh3D()
        : m_materialName( STRINGIZE_STRING_LOCAL( "Material_SkinnedOpaqueUnlit" ) )
        , m_vertexCount( 0 )
        , m_indexCount( 0 )
    {
        mt::ident_m4( &m_renderWorldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    SkinnedMesh3D::~SkinnedMesh3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::setResourceSkinnedMesh3D( const ResourceSkinnedMesh3DPtr & _resource )
    {
        m_resource = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceSkinnedMesh3DPtr & SkinnedMesh3D::getResourceSkinnedMesh3D() const
    {
        return m_resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::setResourceImage( const ResourceImagePtr & _resource )
    {
        m_resourceImage = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & SkinnedMesh3D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::setMaterialName( const ConstString & _materialName )
    {
        m_materialName = _materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & SkinnedMesh3D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::setBoneMatrices( const mt::mat4f * _matrices, uint32_t _count )
    {
        if( _count > MENGINE_SKINNED_MESH3D_PALETTE_MAX )
        {
            _count = MENGINE_SKINNED_MESH3D_PALETTE_MAX;
        }

        m_boneMatrices.assign( _matrices, _matrices + _count );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SkinnedMesh3D::getBoneMatrixCount() const
    {
        return (uint32_t)m_boneMatrices.size();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f * SkinnedMesh3D::getBoneMatrices() const
    {
        return m_boneMatrices.data();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & SkinnedMesh3D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SkinnedMesh3D::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resource, "skinned mesh3d node has no ResourceSkinnedMesh3D" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "skinned mesh3d node has no ResourceImage" );

        if( m_resource->compile() == false )
        {
            return false;
        }

        if( m_resourceImage->compile() == false )
        {
            return false;
        }

        const VectorRenderVertexSkinned3D & vertices = m_resource->getVertices();
        const VectorRenderIndex32 & indices = m_resource->getIndices();

        m_vertexCount = (uint32_t)vertices.size();
        m_indexCount = (uint32_t)indices.size();

        if( m_vertexCount == 0 || m_indexCount == 0 )
        {
            LOGGER_ERROR( "skinned mesh3d resource has empty vertices/indices (v=%u i=%u)"
                , m_vertexCount
                , m_indexCount
            );

            return false;
        }

        m_vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( sizeof( RenderVertexSkinned3D ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "invalid create vertex buffer" );

        if( m_vertexBuffer->resize( m_vertexCount ) == false )
        {
            return false;
        }

        if( m_vertexBuffer->draw( vertices.data(), 0, m_vertexCount ) == false )
        {
            LOGGER_ERROR( "skinned mesh3d failed to upload vertex buffer" );

            return false;
        }

        m_indexBuffer = RENDER_SYSTEM()
            ->createIndexBuffer( sizeof( RenderIndex32 ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "invalid create index buffer" );

        if( m_indexBuffer->resize( m_indexCount ) == false )
        {
            return false;
        }

        if( m_indexBuffer->draw( indices.data(), 0, m_indexCount ) == false )
        {
            LOGGER_ERROR( "skinned mesh3d failed to upload index buffer" );

            return false;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture( 0 );

        m_material = RENDERMATERIAL_SERVICE()
            ->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "skinned mesh3d failed to get material '%s'"
            , m_materialName.c_str()
        );

        m_programVariable = RENDER_SYSTEM()
            ->createProgramVariableDynamic( 1, 0, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_programVariable, "skinned mesh3d failed to create program variable" );

        m_paletteUpload.assign( MENGINE_SKINNED_MESH3D_PALETTE_MAX * Detail::SKINNED_MESH3D_FLOATS_PER_BONE, 0.f );

        if( m_boneMatrices.empty() == true )
        {
            uint32_t boneCount = m_resource->getBoneCount();

            if( boneCount > MENGINE_SKINNED_MESH3D_PALETTE_MAX )
            {
                boneCount = MENGINE_SKINNED_MESH3D_PALETTE_MAX;
            }

            m_boneMatrices.resize( boneCount );

            for( mt::mat4f & m : m_boneMatrices )
            {
                mt::ident_m4( &m );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::_release()
    {
        m_material = nullptr;
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
        m_programVariable = nullptr;

        m_vertexCount = 0;
        m_indexCount = 0;

        m_boneMatrices.clear();
        m_paletteUpload.clear();

        if( m_resource != nullptr )
        {
            m_resource->release();
        }

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::uploadBonePalette_() const
    {
        uint32_t boneCount = (uint32_t)m_boneMatrices.size();

        if( boneCount == 0 )
        {
            return;
        }

        if( boneCount > MENGINE_SKINNED_MESH3D_PALETTE_MAX )
        {
            boneCount = MENGINE_SKINNED_MESH3D_PALETTE_MAX;
        }

        float * dst = m_paletteUpload.data();

        for( uint32_t b = 0; b != boneCount; ++b )
        {
            const mt::mat4f & m = m_boneMatrices[b];

            dst[0]  = m.v0.x;  dst[1]  = m.v1.x;  dst[2]  = m.v2.x;  dst[3]  = m.v3.x;
            dst[4]  = m.v0.y;  dst[5]  = m.v1.y;  dst[6]  = m.v2.y;  dst[7]  = m.v3.y;
            dst[8]  = m.v0.z;  dst[9]  = m.v1.z;  dst[10] = m.v2.z;  dst[11] = m.v3.z;

            dst += Detail::SKINNED_MESH3D_FLOATS_PER_BONE;
        }

        const uint32_t totalRegisters = MENGINE_SKINNED_MESH3D_PALETTE_MAX * 3;

        m_programVariable->setVertexVariables( "BonePalette"
            , Detail::SKINNED_MESH3D_PALETTE_REG_START
            , m_paletteUpload.data()
            , 4
            , totalRegisters );
    }
    //////////////////////////////////////////////////////////////////////////
    void SkinnedMesh3D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "skinned mesh3d render without material" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "skinned mesh3d render without vertex buffer" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "skinned mesh3d render without index buffer" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_programVariable, "skinned mesh3d render without program variable" );

        const mt::mat4f & wm = this->getWorldMatrix();

        if( _context->transformation == nullptr )
        {
            m_renderWorldMatrix = wm;
        }
        else
        {
            const mt::mat4f & parentWM = _context->transformation->getTransformationWorldMatrix();

            m_renderWorldMatrix = wm * parentWM;
        }

        this->uploadBonePalette_();

        constexpr uint32_t worldVec4 = 4u;
        constexpr uint32_t lightingVec4 = Lighting3DServiceInterface::getUploadBlockVec4Count();
        constexpr uint32_t litVec4 = worldVec4 + lightingVec4;
        constexpr uint32_t litFloats = litVec4 * 4u;

        float litBuffer[litFloats];

        litBuffer[0] = m_renderWorldMatrix.v0.x; litBuffer[1] = m_renderWorldMatrix.v0.y; litBuffer[2] = m_renderWorldMatrix.v0.z; litBuffer[3] = m_renderWorldMatrix.v0.w;
        litBuffer[4] = m_renderWorldMatrix.v1.x; litBuffer[5] = m_renderWorldMatrix.v1.y; litBuffer[6] = m_renderWorldMatrix.v1.z; litBuffer[7] = m_renderWorldMatrix.v1.w;
        litBuffer[8] = m_renderWorldMatrix.v2.x; litBuffer[9] = m_renderWorldMatrix.v2.y; litBuffer[10] = m_renderWorldMatrix.v2.z; litBuffer[11] = m_renderWorldMatrix.v2.w;
        litBuffer[12] = m_renderWorldMatrix.v3.x; litBuffer[13] = m_renderWorldMatrix.v3.y; litBuffer[14] = m_renderWorldMatrix.v3.z; litBuffer[15] = m_renderWorldMatrix.v3.w;

        LIGHTING3D_SERVICE()
            ->packUploadBlock( litBuffer + worldVec4 * 4u );

        m_programVariable->setVertexVariables( "SkinnedLit"
            , Detail::SKINNED_MESH3D_LIGHTING_REG_START
            , litBuffer
            , 4u
            , litVec4 );

        RenderContext new_context = *_context;
        new_context.transformation = this;

        _renderPipeline->addRenderMesh( &new_context
            , m_material
            , m_programVariable
            , m_vertexBuffer
            , m_indexBuffer
            , m_vertexCount
            , m_indexCount
            , 0, 0
            , MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}
