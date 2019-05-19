#include "OzzDetail.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive_traits.h"
#include "ozz/base/memory/allocator.h"

#include "Kernel/Range.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        size_t getPartVertexCount( const Part & _part )
        {
            Part::VectorPositions::size_type position_size = _part.positions.size();

            size_t vertex_count = position_size / 3;

            return vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t getPartInfluencesCount( const Part & _part )
        {
            const size_t _vertex_count = getPartVertexCount( _part );

            if( _vertex_count == 0 )
            {
                return 0;
            }

            Part::VectorJointIndices::size_type joint_indices_size = _part.joint_indices.size();

            return joint_indices_size / _vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t getMeshVertexCount( const Mesh & _mesh )
        {
            size_t vertex_count = 0;

            for( const Part & part : _mesh.parts )
            {
                size_t part_vertex_count = getPartVertexCount( part );

                vertex_count += part_vertex_count;
            }

            return vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        ScratchBuffer::ScratchBuffer()
            : m_buffer( nullptr )
            , m_size( 0 )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        ScratchBuffer::~ScratchBuffer()
        {
            ozz::memory::default_allocator()->Deallocate( m_buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        void * ScratchBuffer::Resize( size_t _size )
        {
            if( _size > m_size )
            {
                m_size = _size;
                m_buffer = ozz::memory::default_allocator()->Reallocate( m_buffer, _size, 16 );
            }

            return m_buffer;
        }
    }
}

namespace ozz
{
    namespace io
    {
        void Extern<Mengine::Detail::Part>::Save( OArchive & _archive, const Mengine::Detail::Part * _parts, size_t _count )
        {
            for( const Mengine::Detail::Part & part : Mengine::Helper::makeRange( _parts, _count ) )
            {
                _archive << part.positions;
                _archive << part.normals;
                _archive << part.tangents;
                _archive << part.uvs;
                _archive << part.colors;
                _archive << part.joint_indices;
                _archive << part.joint_weights;
            }
        }

        void Extern<Mengine::Detail::Part>::Load( IArchive & _archive, Mengine::Detail::Part * _parts, size_t _count, uint32_t _version )
        {
            MENGINE_UNUSED( _version );

            for( Mengine::Detail::Part & part : Mengine::Helper::makeRange( _parts, _count ) )
            {
                _archive >> part.positions;
                _archive >> part.normals;
                _archive >> part.tangents;
                _archive >> part.uvs;
                _archive >> part.colors;
                _archive >> part.joint_indices;
                _archive >> part.joint_weights;
            }
        }

        void Extern<Mengine::Detail::Mesh>::Save( OArchive & _archive, const Mengine::Detail::Mesh * _meshes, size_t _count )
        {
            for( const Mengine::Detail::Mesh & mesh : Mengine::Helper::makeRange( _meshes, _count ) )
            {
                _archive << mesh.parts;
                _archive << mesh.triangle_indices;
                _archive << mesh.joint_remaps;
                _archive << mesh.inverse_bind_poses;
            }
        }

        void Extern<Mengine::Detail::Mesh>::Load( IArchive & _archive, Mengine::Detail::Mesh * _meshes, size_t _count, uint32_t _version )
        {
            MENGINE_UNUSED( _version );

            for( Mengine::Detail::Mesh & mesh : Mengine::Helper::makeRange( _meshes, _count ) )
            {
                _archive >> mesh.parts;
                _archive >> mesh.triangle_indices;
                _archive >> mesh.joint_remaps;
                _archive >> mesh.inverse_bind_poses;
            }
        }
    }

}
