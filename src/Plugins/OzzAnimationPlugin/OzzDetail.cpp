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
        uint32_t getPartVertexCount( const Part & _part )
        {
            Part::VectorPositions::size_type position_size = _part.positions.size();

            uint32_t vertex_count = (uint32_t)position_size / 3;

            return vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getPartInfluencesCount( const Part & _part )
        {
            uint32_t _vertex_count = Detail::getPartVertexCount( _part );

            if( _vertex_count == 0 )
            {
                return 0;
            }

            Part::VectorJointIndices::size_type joint_indices_size = _part.joint_indices.size();

            return (uint32_t)joint_indices_size / _vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getMeshVertexCount( const Mesh & _mesh )
        {
            uint32_t vertex_count = 0;

            for( const Part & part : _mesh.parts )
            {
                uint32_t part_vertex_count = Detail::getPartVertexCount( part );

                vertex_count += part_vertex_count;
            }

            return vertex_count;
        }
        //////////////////////////////////////////////////////////////////////////
        uint16_t getMeshHighestJointIndex( const Mesh & _mesh )
        {
            if( _mesh.joint_remaps.empty() == true )
            {
                return 0;
            }
            
            uint16_t index = _mesh.joint_remaps.back();

            return index;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

namespace ozz
{
    namespace io
    {
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
    }

}
