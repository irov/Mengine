#pragma once

#include "Kernel/Vector.h"

#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/simd_math_archive.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/base/containers/vector.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const size_t ozz_normals_size = sizeof( float ) * 3;
        const size_t ozz_tangents_size = sizeof( float ) * 6;
        const size_t ozz_positions_size = sizeof( float ) * 9;
        const size_t ozz_colors_size = sizeof( uint8_t ) * 4;
        const size_t ozz_uvs_size = sizeof( float ) * 2;
        //////////////////////////////////////////////////////////////////////////
        const size_t ozz_vertex_size = ozz_normals_size + ozz_tangents_size + ozz_positions_size + ozz_colors_size + ozz_uvs_size;
        //////////////////////////////////////////////////////////////////////////
        struct Part
        {
            typedef ozz::vector<float> VectorPositions;
            VectorPositions positions;
            enum
            {
                kPositionsCpnts = 3
            };  // x, y, z components

            typedef ozz::vector<float> VectorNormals;
            VectorNormals normals;
            enum
            {
                kNormalsCpnts = 3
            };  // x, y, z components

            typedef ozz::vector<float> VectorTangents;
            VectorTangents tangents;
            enum
            {
                kTangentsCpnts = 4
            };  // x, y, z, right or left handed.

            typedef ozz::vector<float> VectorUVs;
            VectorUVs uvs;  // u, v components
            enum
            {
                kUVsCpnts = 2
            };

            typedef ozz::vector<uint8_t> VectorColors;
            VectorColors colors;
            enum
            {
                kColorsCpnts = 4
            };  // r, g, b, a components

            typedef ozz::vector<uint16_t> VectorJointIndices;
            VectorJointIndices joint_indices;  // Stride equals influences_count

            typedef ozz::vector<float> VectorJointWeights;
            VectorJointWeights joint_weights;  // Stride equals influences_count - 1
        };
        //////////////////////////////////////////////////////////////////////////
        struct Mesh
        {
            typedef ozz::vector<Part> VectorParts;
            VectorParts parts;

            // Triangles indices. Indices are shared across all parts.
            typedef ozz::vector<uint16_t> VectorTriangleIndices;
            VectorTriangleIndices triangle_indices;

            typedef ozz::vector<uint16_t> VectorJointRemaps;
            VectorJointRemaps joint_remaps;

            // Inverse bind-pose matrices. These are only available for skinned meshes.
            typedef ozz::vector<ozz::math::Float4x4> VectorInversBindPoses;
            VectorInversBindPoses inverse_bind_poses;
        };
        //////////////////////////////////////////////////////////////////////////
        uint32_t getPartVertexCount( const Part & _part );
        uint32_t getPartInfluencesCount( const Part & _part );
        //////////////////////////////////////////////////////////////////////////
        uint32_t getMeshVertexCount( const Mesh & _mesh );
        uint16_t getMeshHighestJointIndex( const Mesh & _mesh );
        //////////////////////////////////////////////////////////////////////////
    }
}

namespace ozz
{
    namespace io
    {
        //////////////////////////////////////////////////////////////////////////
        OZZ_IO_TYPE_TAG( "ozz-sample-Mesh-Part", Mengine::Detail::Part );
        OZZ_IO_TYPE_VERSION( 1, Mengine::Detail::Part );
        //////////////////////////////////////////////////////////////////////////
        OZZ_IO_TYPE_TAG( "ozz-sample-Mesh", Mengine::Detail::Mesh );
        OZZ_IO_TYPE_VERSION( 1, Mengine::Detail::Mesh );
        //////////////////////////////////////////////////////////////////////////
        template<>
        struct Extern<Mengine::Detail::Part>
        {
            static void Save( OArchive & _archive, const Mengine::Detail::Part * _parts, size_t _count );
            static void Load( IArchive & _archive, Mengine::Detail::Part * _parts, size_t _count, uint32_t _version );
        };
        //////////////////////////////////////////////////////////////////////////
        template<>
        struct Extern<Mengine::Detail::Mesh>
        {
            static void Save( OArchive & _archive, const Mengine::Detail::Mesh * _meshes, size_t _count );
            static void Load( IArchive & _archive, Mengine::Detail::Mesh * _meshes, size_t _count, uint32_t _version );
        };
        //////////////////////////////////////////////////////////////////////////
    }
}
