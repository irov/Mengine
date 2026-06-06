#include "BlobShadow3D.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/RenderVertex3D.h"
#include "Kernel/RenderIndex32.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static const uint32_t BLOB_SHADOW3D_VERTEX_COUNT = 4;
        static const uint32_t BLOB_SHADOW3D_INDEX_COUNT = 6;

        static const RenderIndex32 BLOB_SHADOW3D_INDICES[BLOB_SHADOW3D_INDEX_COUNT] = {
            0, 1, 2,
            0, 2, 3,
        };
    }
    //////////////////////////////////////////////////////////////////////////
    BlobShadow3D::BlobShadow3D()
        : m_materialName( STRINGIZE_STRING_LOCAL( "Material_BlobShadow" ) )
        , m_width( 1.f )
        , m_depth( 1.f )
        , m_color( 0xFFFFFFFFu )
    {
        mt::ident_m4( &m_renderWorldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    BlobShadow3D::~BlobShadow3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::setResourceImage( const ResourceImagePtr & _resource )
    {
        m_resourceImage = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & BlobShadow3D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::setMaterialName( const ConstString & _materialName )
    {
        m_materialName = _materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & BlobShadow3D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::setShadowSize( float _width, float _depth )
    {
        m_width = _width;
        m_depth = _depth;

        if( m_vertexBuffer != nullptr )
        {
            this->rebuildQuad_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float BlobShadow3D::getShadowWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    float BlobShadow3D::getShadowDepth() const
    {
        return m_depth;
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::setShadowColor( float _r, float _g, float _b, float _a )
    {
        auto pack = []( float v ) -> uint8_t
            {
                if( v < 0.f ) v = 0.f;
                if( v > 1.f ) v = 1.f;
                return (uint8_t)(v * 255.f + 0.5f);
            };

        uint8_t r = pack( _r );
        uint8_t g = pack( _g );
        uint8_t b = pack( _b );
        uint8_t a = pack( _a );

        m_color = ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;

        if( m_vertexBuffer != nullptr )
        {
            this->rebuildQuad_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & BlobShadow3D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BlobShadow3D::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "blob shadow 3d node has no ResourceImage" );

        if( m_resourceImage->compile() == false )
        {
            return false;
        }

        m_vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( sizeof( RenderVertex3D ), BT_DYNAMIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "blob shadow 3d failed to create vertex buffer" );

        if( m_vertexBuffer->resize( Detail::BLOB_SHADOW3D_VERTEX_COUNT ) == false )
        {
            return false;
        }

        m_indexBuffer = RENDER_SYSTEM()
            ->createIndexBuffer( sizeof( RenderIndex32 ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "blob shadow 3d failed to create index buffer" );

        if( m_indexBuffer->resize( Detail::BLOB_SHADOW3D_INDEX_COUNT ) == false )
        {
            return false;
        }

        if( m_indexBuffer->draw( Detail::BLOB_SHADOW3D_INDICES, 0, Detail::BLOB_SHADOW3D_INDEX_COUNT ) == false )
        {
            LOGGER_ERROR( "blob shadow 3d failed to upload index buffer" );

            return false;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture( 0 );

        m_material = RENDERMATERIAL_SERVICE()
            ->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "blob shadow 3d failed to get material '%s'"
            , m_materialName.c_str()
        );

        this->rebuildQuad_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::_release()
    {
        m_material = nullptr;
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::rebuildQuad_()
    {
        if( m_vertexBuffer == nullptr )
        {
            return;
        }

        const float hx = m_width * 0.5f;
        const float hz = m_depth * 0.5f;

        RenderVertex3D verts[Detail::BLOB_SHADOW3D_VERTEX_COUNT];

        verts[0].position = mt::vec3f( -hx, 0.f, -hz );
        verts[1].position = mt::vec3f(  hx, 0.f, -hz );
        verts[2].position = mt::vec3f(  hx, 0.f,  hz );
        verts[3].position = mt::vec3f( -hx, 0.f,  hz );

        for( uint32_t i = 0; i != Detail::BLOB_SHADOW3D_VERTEX_COUNT; ++i )
        {
            verts[i].normal = mt::vec3f( 0.f, 1.f, 0.f );
            verts[i].tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
            verts[i].color = m_color;
        }

        verts[0].uv[0] = mt::vec2f( 0.f, 0.f );
        verts[1].uv[0] = mt::vec2f( 1.f, 0.f );
        verts[2].uv[0] = mt::vec2f( 1.f, 1.f );
        verts[3].uv[0] = mt::vec2f( 0.f, 1.f );

        for( uint32_t i = 0; i != Detail::BLOB_SHADOW3D_VERTEX_COUNT; ++i )
        {
            verts[i].uv[1] = verts[i].uv[0];
        }

        if( m_vertexBuffer->draw( verts, 0, Detail::BLOB_SHADOW3D_VERTEX_COUNT ) == false )
        {
            LOGGER_ERROR( "blob shadow 3d failed to upload vertex buffer" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BlobShadow3D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_material, "blob shadow 3d render without material" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "blob shadow 3d render without vertex buffer" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "blob shadow 3d render without index buffer" );

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

        _renderPipeline->addRenderMesh( &new_context
            , m_material
            , nullptr
            , m_vertexBuffer
            , m_indexBuffer
            , Detail::BLOB_SHADOW3D_VERTEX_COUNT
            , Detail::BLOB_SHADOW3D_INDEX_COUNT
            , 0, 0
            , MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}
