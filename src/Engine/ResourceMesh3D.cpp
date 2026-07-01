#include "ResourceMesh3D.h"

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
        struct MengMeshHeader
        {
            char     magic[4];
            uint32_t version;
            uint32_t vertexCount;
            uint32_t indexCount;
            uint32_t flags;
            uint32_t reserved;
        };
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t MENGMESH_VERSION_CURRENT = 1;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMesh3D::ResourceMesh3D()
    {
        mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMesh3D::~ResourceMesh3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorRenderVertex3D & ResourceMesh3D::getVertices() const
    {
        return m_vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorRenderIndex32 & ResourceMesh3D::getIndices() const
    {
        return m_indices;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box3f & ResourceMesh3D::getBoundingBox() const
    {
        return m_boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMesh3D::setVertices( const VectorRenderVertex3D & _vertices )
    {
        m_vertices = _vertices;

        this->recomputeBoundingBox_();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMesh3D::setIndices( const VectorRenderIndex32 & _indices )
    {
        m_indices = _indices;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMesh3D::recomputeBoundingBox_()
    {
        if( m_vertices.empty() == true )
        {
            mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );

            return;
        }

        mt::reset( &m_boundingBox, m_vertices.front().position );

        for( const RenderVertex3D & v : m_vertices )
        {
            mt::add_internal_point( &m_boundingBox, v.position );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMesh3D::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        if( content == nullptr )
        {
            if( m_vertices.empty() == true || m_indices.empty() == true )
            {
                LOGGER_ERROR( "mesh3d '%s' has no content and no in-memory geometry"
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

        if( total_size < sizeof( Detail::MengMeshHeader ) )
        {
            LOGGER_ERROR( "mesh3d '%s' file too small (%zu bytes)"
                , Helper::getContentFullPath( content ).c_str()
                , total_size
            );

            return false;
        }

        Detail::MengMeshHeader header;
        StdString::memcpy_pod( &header, buffer );

        if( header.magic[0] != 'M' || header.magic[1] != 'M'
            || header.magic[2] != '3' || header.magic[3] != 'D' )
        {
            LOGGER_ERROR( "mesh3d '%s' invalid magic"
                , Helper::getContentFullPath( content ).c_str()
            );

            return false;
        }

        if( header.version != Detail::MENGMESH_VERSION_CURRENT )
        {
            LOGGER_ERROR( "mesh3d '%s' unsupported version %u (expected %u)"
                , Helper::getContentFullPath( content ).c_str()
                , header.version
                , Detail::MENGMESH_VERSION_CURRENT
            );

            return false;
        }

        size_t vbBytes = (size_t)header.vertexCount * sizeof( RenderVertex3D );
        size_t ibBytes = (size_t)header.indexCount * sizeof( RenderIndex32 );

        size_t expected = sizeof( Detail::MengMeshHeader ) + vbBytes + ibBytes;

        if( total_size < expected )
        {
            LOGGER_ERROR( "mesh3d '%s' truncated: have %zu bytes, expected %zu"
                , Helper::getContentFullPath( content ).c_str()
                , total_size
                , expected
            );

            return false;
        }

        m_vertices.resize( header.vertexCount );
        m_indices.resize( header.indexCount );

        if( header.vertexCount > 0 )
        {
            StdString::memcpy_pod( m_vertices.data(), buffer + sizeof( Detail::MengMeshHeader ), header.vertexCount );
        }

        if( header.indexCount > 0 )
        {
            StdString::memcpy_pod( m_indices.data(), buffer + sizeof( Detail::MengMeshHeader ) + vbBytes, header.indexCount );
        }

        this->recomputeBoundingBox_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMesh3D::_release()
    {
        if( this->getContent() == nullptr )
        {
            return;
        }

        m_vertices.clear();
        m_indices.clear();

        mt::reset( &m_boundingBox, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
}
