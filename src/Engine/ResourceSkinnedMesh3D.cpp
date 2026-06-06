#include "ResourceSkinnedMesh3D.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct MengSkinnedMeshHeader
        {
            char     magic[4];
            uint32_t version;
            uint32_t vertexCount;
            uint32_t indexCount;
            uint32_t boneCount;
            uint32_t flags;
            uint32_t reserved;
        };
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t MENGMESHS_VERSION_CURRENT = 1;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSkinnedMesh3D::ResourceSkinnedMesh3D()
    {
        mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSkinnedMesh3D::~ResourceSkinnedMesh3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorRenderVertexSkinned3D & ResourceSkinnedMesh3D::getVertices() const
    {
        return m_vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorRenderIndex32 & ResourceSkinnedMesh3D::getIndices() const
    {
        return m_indices;
    }
    //////////////////////////////////////////////////////////////////////////
    const Vector<mt::mat4f> & ResourceSkinnedMesh3D::getInverseBindMatrices() const
    {
        return m_inverseBindMatrices;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box3f & ResourceSkinnedMesh3D::getBoundingBox() const
    {
        return m_boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceSkinnedMesh3D::getBoneCount() const
    {
        return (uint32_t)m_inverseBindMatrices.size();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSkinnedMesh3D::setVertices( const VectorRenderVertexSkinned3D & _vertices )
    {
        m_vertices = _vertices;

        this->recomputeBoundingBox_();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSkinnedMesh3D::setIndices( const VectorRenderIndex32 & _indices )
    {
        m_indices = _indices;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSkinnedMesh3D::setInverseBindMatrices( const Vector<mt::mat4f> & _matrices )
    {
        m_inverseBindMatrices = _matrices;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSkinnedMesh3D::recomputeBoundingBox_()
    {
        if( m_vertices.empty() == true )
        {
            mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );

            return;
        }

        mt::reset( &m_boundingBox, m_vertices.front().position );

        for( const RenderVertexSkinned3D & v : m_vertices )
        {
            mt::add_internal_point( &m_boundingBox, v.position );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSkinnedMesh3D::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        if( content == nullptr )
        {
            if( m_vertices.empty() == true || m_indices.empty() == true || m_inverseBindMatrices.empty() == true )
            {
                LOGGER_ERROR( "skinned mesh3d '%s' has no content and no in-memory geometry"
                    , this->getName().c_str()
                );

                return false;
            }

            return true;
        }

        InputStreamInterfacePtr stream = content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open stream '%s'"
            , Helper::getContentFullPath( content ).c_str()
        );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory '%s'"
            , Helper::getContentFullPath( content ).c_str()
        );

        content->closeInputStreamFile( stream );

        const uint8_t * buffer = (const uint8_t *)memory->getBuffer();
        size_t total_size = memory->getSize();

        if( total_size < sizeof( Detail::MengSkinnedMeshHeader ) )
        {
            LOGGER_ERROR( "skinned mesh3d '%s' file too small (%zu bytes)"
                , Helper::getContentFullPath( content ).c_str()
                , total_size
            );

            return false;
        }

        Detail::MengSkinnedMeshHeader header;
        StdString::memcpy( &header, buffer, sizeof( Detail::MengSkinnedMeshHeader ) );

        if( header.magic[0] != 'M' || header.magic[1] != 'M'
            || header.magic[2] != 'S' || header.magic[3] != 'K' )
        {
            LOGGER_ERROR( "skinned mesh3d '%s' invalid magic"
                , Helper::getContentFullPath( content ).c_str()
            );

            return false;
        }

        if( header.version != Detail::MENGMESHS_VERSION_CURRENT )
        {
            LOGGER_ERROR( "skinned mesh3d '%s' unsupported version %u (expected %u)"
                , Helper::getContentFullPath( content ).c_str()
                , header.version
                , Detail::MENGMESHS_VERSION_CURRENT
            );

            return false;
        }

        size_t vbBytes  = (size_t)header.vertexCount * sizeof( RenderVertexSkinned3D );
        size_t ibBytes  = (size_t)header.indexCount  * sizeof( RenderIndex32 );
        size_t ibmBytes = (size_t)header.boneCount   * sizeof( mt::mat4f );

        size_t expected = sizeof( Detail::MengSkinnedMeshHeader ) + vbBytes + ibBytes + ibmBytes;

        if( total_size < expected )
        {
            LOGGER_ERROR( "skinned mesh3d '%s' truncated: have %zu bytes, expected %zu"
                , Helper::getContentFullPath( content ).c_str()
                , total_size
                , expected
            );

            return false;
        }

        m_vertices.resize( header.vertexCount );
        m_indices.resize( header.indexCount );
        m_inverseBindMatrices.resize( header.boneCount );

        const uint8_t * cursor = buffer + sizeof( Detail::MengSkinnedMeshHeader );

        if( header.vertexCount > 0 )
        {
            StdString::memcpy( m_vertices.data(), cursor, vbBytes );
            cursor += vbBytes;
        }

        if( header.indexCount > 0 )
        {
            StdString::memcpy( m_indices.data(), cursor, ibBytes );
            cursor += ibBytes;
        }

        if( header.boneCount > 0 )
        {
            StdString::memcpy( m_inverseBindMatrices.data(), cursor, ibmBytes );
        }

        this->recomputeBoundingBox_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSkinnedMesh3D::_release()
    {
        if( this->getContent() == nullptr )
        {
            return;
        }

        m_vertices.clear();
        m_indices.clear();
        m_inverseBindMatrices.clear();

        mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
}
