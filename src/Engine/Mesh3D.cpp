#include "Mesh3D.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Mesh3D::Mesh3D()
        : m_materialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueUnlit" ) )
        , m_vertexCount( 0 )
        , m_indexCount( 0 )
    {
        mt::ident_m4( &m_renderWorldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    Mesh3D::~Mesh3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh3D::setResourceMesh3D( const ResourceMesh3DPtr & _resource )
    {
        m_resourceMesh = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceMesh3DPtr & Mesh3D::getResourceMesh3D() const
    {
        return m_resourceMesh;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh3D::setResourceImage( const ResourceImagePtr & _resource )
    {
        m_resourceImage = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Mesh3D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh3D::setMaterialName( const ConstString & _materialName )
    {
        m_materialName = _materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Mesh3D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & Mesh3D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Mesh3D::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceMesh, "mesh3d node has no ResourceMesh3D" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "mesh3d node has no ResourceImage" );

        if( m_resourceMesh->compile() == false )
        {
            return false;
        }

        if( m_resourceImage->compile() == false )
        {
            return false;
        }

        const VectorRenderVertex3D & vertices = m_resourceMesh->getVertices();
        const VectorRenderIndex32 & indices = m_resourceMesh->getIndices();

        m_vertexCount = (uint32_t)vertices.size();
        m_indexCount = (uint32_t)indices.size();

        if( m_vertexCount == 0 || m_indexCount == 0 )
        {
            LOGGER_ERROR( "mesh3d resource has empty vertices/indices (v=%u i=%u)"
                , m_vertexCount
                , m_indexCount
            );

            return false;
        }

        m_vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( sizeof( RenderVertex3D ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "invalid create vertex buffer" );

        if( m_vertexBuffer->resize( m_vertexCount ) == false )
        {
            return false;
        }

        if( m_vertexBuffer->draw( vertices.data(), 0, m_vertexCount ) == false )
        {
            LOGGER_ERROR( "mesh3d failed to upload vertex buffer" );

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
            LOGGER_ERROR( "mesh3d failed to upload index buffer" );

            return false;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture( 0 );

        m_material = RENDERMATERIAL_SERVICE()
            ->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "mesh3d failed to get material '%s'"
            , m_materialName.c_str()
        );

        m_programVariable = RENDER_SYSTEM()
            ->createProgramVariableDynamic( 1, 0, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_programVariable, "mesh3d failed to create program variable" );

        m_uniformUpload.assign( (4 + Lighting3DServiceInterface::getUploadBlockVec4Count()) * 4u, 0.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh3D::_release()
    {
        m_material = nullptr;
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
        m_programVariable = nullptr;

        m_uniformUpload.clear();

        m_vertexCount = 0;
        m_indexCount = 0;

        if( m_resourceMesh != nullptr )
        {
            m_resourceMesh->release();
        }

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh3D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "mesh3d render without material" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "mesh3d render without vertex buffer" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "mesh3d render without index buffer" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_programVariable, "mesh3d render without program variable" );

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

        RenderContext new_context = *_context;
        new_context.transformation = this;

        float * dst = m_uniformUpload.data();
        dst[ 0] = m_renderWorldMatrix.v0.x; dst[ 1] = m_renderWorldMatrix.v0.y; dst[ 2] = m_renderWorldMatrix.v0.z; dst[ 3] = m_renderWorldMatrix.v0.w;
        dst[ 4] = m_renderWorldMatrix.v1.x; dst[ 5] = m_renderWorldMatrix.v1.y; dst[ 6] = m_renderWorldMatrix.v1.z; dst[ 7] = m_renderWorldMatrix.v1.w;
        dst[ 8] = m_renderWorldMatrix.v2.x; dst[ 9] = m_renderWorldMatrix.v2.y; dst[10] = m_renderWorldMatrix.v2.z; dst[11] = m_renderWorldMatrix.v2.w;
        dst[12] = m_renderWorldMatrix.v3.x; dst[13] = m_renderWorldMatrix.v3.y; dst[14] = m_renderWorldMatrix.v3.z; dst[15] = m_renderWorldMatrix.v3.w;

        LIGHTING3D_SERVICE()
            ->packUploadBlock( dst + 16 );

        const uint32_t totalVec4 = 4u + Lighting3DServiceInterface::getUploadBlockVec4Count();

        m_programVariable->setVertexVariables( "Mesh3DUniforms", 4u, dst, 4u, totalVec4 );

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
