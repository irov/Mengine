#include "NodeOzzAnimation.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "ResourceOzzAnimation.h"

#include "Kernel/RenderUtils.h"
#include "Kernel/Document.h"

#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/containers/vector.h"

#include "ozz/animation/runtime/skeleton_utils.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"

#include "ozz/geometry/runtime/skinning_job.h"

#include <string.h>
#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_positions_size = sizeof( float ) * 3;
    //static const uint32_t ozz_normals_size = sizeof( float ) * 3;
    //static const uint32_t ozz_tangents_size = sizeof( float ) * 4;
    //static const uint32_t ozz_colors_size = sizeof( uint8_t ) * 4;
    //static const uint32_t ozz_uvs_size = sizeof( float ) * 2;
    static const uint32_t ozz_positions_size = sizeof( float ) * 3;
    static const uint32_t ozz_colors_size = sizeof( uint8_t ) * 4;
    static const uint32_t ozz_uvs_size = sizeof( float ) * 2;
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_vertex_stride = ozz_positions_size + ozz_normals_size + ozz_tangents_size + ozz_colors_size + ozz_uvs_size;
    static const uint32_t ozz_vertex_stride = ozz_positions_size + ozz_colors_size + ozz_uvs_size;
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_positions_offset = 0;
    //static const uint32_t ozz_normals_offset = ozz_positions_offset + ozz_positions_size;
    //static const uint32_t ozz_tangents_offset = ozz_normals_offset + ozz_normals_size;
    //static const uint32_t ozz_colors_offset = ozz_tangents_offset + ozz_tangents_size;
    //static const uint32_t ozz_uvs_offset = ozz_colors_offset + ozz_colors_size;
    static const uint32_t ozz_positions_offset = 0;
    static const uint32_t ozz_colors_offset = ozz_positions_offset + ozz_positions_size;
    static const uint32_t ozz_uvs_offset = ozz_colors_offset + ozz_colors_size;
    //////////////////////////////////////////////////////////////////////////
    NodeOzzAnimation::NodeOzzAnimation()
        : m_upperBodyRoot( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeOzzAnimation::~NodeOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::setResourceOzzImage( const ResourceImagePtr & _resource )
    {
        if( m_resourceImage == _resource )
        {
            return;
        }

        this->recompile( [this, _resource]() {this->m_resourceImage = _resource; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & NodeOzzAnimation::getResourceOzzImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::setResourceOzzSkeleton( const ResourcePtr & _resource )
    {
        if( m_resourceSkeleton == _resource )
        {
            return;
        }

        this->recompile( [this, _resource]() {this->m_resourceSkeleton = _resource; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & NodeOzzAnimation::getResourceOzzSkeleton() const
    {
        return m_resourceSkeleton;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::setResourceOzzMesh( const ResourcePtr & _resource )
    {
        if( m_resourceMesh == _resource )
        {
            return;
        }

        this->recompile( [this, _resource]() {this->m_resourceMesh = _resource; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & NodeOzzAnimation::getResourceOzzMesh() const
    {
        return m_resourceMesh;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::addOzzAnimationSampler( const ConstString & _name, const SamplerOzzAnimationInterfacePtr & _sampler )
    {
        SamplerDesc desc;
        desc.name = _name;
        desc.sampler = _sampler;

        m_samplerOzzAnimations.push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::removeOzzAnimationSampler( const ConstString & _name )
    {
        VectorSamplerOzzAnimations::iterator it_found = std::find_if( m_samplerOzzAnimations.begin(), m_samplerOzzAnimations.end()
            , [_name]( const SamplerDesc & _desc ) { return _desc.name == _name; }
        );

        if( it_found == m_samplerOzzAnimations.end() )
        {
            return;
        }

        m_samplerOzzAnimations.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerOzzAnimationInterfacePtr & NodeOzzAnimation::findOzzAnimationSampler( const ConstString & _name ) const
    {
        for( const SamplerDesc & desc : m_samplerOzzAnimations )
        {
            if( desc.name != _name )
            {
                continue;
            }

            return desc.sampler;
        }

        return SamplerOzzAnimationInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NodeOzzAnimation::getOzzAnimationSamplerCount() const
    {
        VectorSamplerOzzAnimations::size_type count = m_samplerOzzAnimations.size();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerOzzAnimationInterfacePtr & NodeOzzAnimation::getOzzAnimationSampler( uint32_t _index ) const
    {
        const SamplerDesc & desc = m_samplerOzzAnimations[_index];

        const SamplerOzzAnimationInterfacePtr & sampler = desc.sampler;

        return sampler;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & NodeOzzAnimation::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeOzzAnimation::_compile()
    {
        if( m_resourceImage->compile() == false )
        {
            return false;
        }

        if( m_resourceSkeleton->compile() == false )
        {
            return false;
        }

        if( m_resourceMesh->compile() == false )
        {
            return false;
        }

        ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

        const ozz::animation::Skeleton & skeleton = m_resourceSkeleton->getOzzSkeleton();

        int32_t num_soa_joints = skeleton.num_soa_joints();
        int32_t num_joints = skeleton.num_joints();

        m_models = allocator->AllocateRange<ozz::math::Float4x4>( num_joints );

        m_skinningMatrices = allocator->AllocateRange<ozz::math::Float4x4>( num_joints );

        m_upperBodyJointWeights = allocator->AllocateRange<ozz::math::SimdFloat4>( num_soa_joints );

        m_blendedLocals = allocator->AllocateRange<ozz::math::SoaTransform>( num_soa_joints );

        // Finds the "Spine1" joint in the joint hierarchy.
        const ozz::Range<const char* const> & joint_names = skeleton.joint_names();

        for( int32_t i = 0; i < num_joints; ++i )
        {
            const Char * joint_name = joint_names[i];

            if( strstr( joint_name, "Spine1" ) != nullptr )
            {
                m_upperBodyRoot = i;
                break;
            }
        }

        for( int32_t i = 0; i != num_soa_joints; ++i )
        {
            m_upperBodyJointWeights[i] = ozz::math::simd_float4::zero();
        }

        // Extracts the list of children of the shoulder.
        ozz::animation::JointsIterator it;
        ozz::animation::IterateJointsDF( skeleton, m_upperBodyRoot, &it );

        float upper_body_joint_weight_setting = 1.f;

        // Sets the weight_setting of all the joints children of the arm to 1. Note
        // that weights are stored in SoA format.
        for( int32_t i = 0; i != it.num_joints; ++i )
        {
            const int32_t joint_id = it.joints[i];

            // Updates upper body animation sampler joint weights.
            ozz::math::SimdFloat4& weight_setting = m_upperBodyJointWeights[joint_id / 4];

            weight_setting = ozz::math::SetI( weight_setting, joint_id % 4, upper_body_joint_weight_setting );
        }

        m_vertexMemory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        RenderVertexBufferInterfacePtr vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( ozz_vertex_stride, BT_STREAM, MENGINE_DOCUMENT_FUNCTION );

        const Detail::Mesh & ozz_mesh = m_resourceMesh->getMesh();

        int32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

        vertexBuffer->resize( vertex_count );

        m_vertexBuffer = vertexBuffer;

        RenderIndexBufferInterfacePtr indexStream = RENDER_SYSTEM()
            ->createIndexBuffer( sizeof( RenderIndex ), BT_STREAM, MENGINE_DOCUMENT_FUNCTION );

        const Detail::Mesh::VectorTriangleIndices & triangle_indices = ozz_mesh.triangle_indices;

        size_t triangle_indices_buffer_size = triangle_indices.size();

        indexStream->resize( triangle_indices_buffer_size );

        m_indexBuffer = indexStream;

        RenderTextureInterfacePtr texture = m_resourceImage->getTexture();

        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getMaterial( STRINGIZE_STRING_LOCAL( "Ozz_Texture_Blend" ), PT_TRIANGLELIST, 1, &texture, MENGINE_DOCUMENT_FUNCTION );

        if( material == nullptr )
        {
            return false;
        }

        m_material = material;

        for( const SamplerDesc & desc : m_samplerOzzAnimations )
        {
            const SamplerOzzAnimationPtr & sampler = desc.sampler;

            if( sampler->compile() == false )
            {
                return false;
            }
        }

        this->updateAnimation_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::_release()
    {
        ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

        allocator->Deallocate( m_models );
        allocator->Deallocate( m_skinningMatrices );
        allocator->Deallocate( m_upperBodyJointWeights );
        allocator->Deallocate( m_blendedLocals );

        m_vertexMemory = nullptr;
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
        m_material = nullptr;

        for( const SamplerDesc & desc : m_samplerOzzAnimations )
        {
            const SamplerOzzAnimationPtr & sampler = desc.sampler;

            sampler->release();
        }

        m_samplerOzzAnimations.clear();

        m_resourceImage->release();
        m_resourceSkeleton->release();
        m_resourceMesh->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::_update( const UpdateContext * _context )
    {
        for( const SamplerDesc & desc : m_samplerOzzAnimations )
        {
            const SamplerOzzAnimationPtr & sampler = desc.sampler;

            sampler->update( _context );
        }

        this->updateAnimation_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::updateAnimation_()
    {
        ozz::animation::BlendingJob::Layer layers[32];
        uint32_t layers_iterator = 0;

        for( const SamplerDesc & desc : m_samplerOzzAnimations )
        {
            const SamplerOzzAnimationPtr & sampler = desc.sampler;

            if( sampler->isCompile() == false )
            {
                continue;
            }

            layers[layers_iterator].transform = sampler->getLocals();
            layers[layers_iterator].weight = sampler->getWeight();

            layers_iterator++;
        }

        float threshold = ozz::animation::BlendingJob().threshold;

        const ozz::animation::Skeleton & ozz_skeleton = m_resourceSkeleton->getOzzSkeleton();

        ozz::animation::BlendingJob blend_job;
        blend_job.threshold = threshold;
        blend_job.layers = ozz::Range<ozz::animation::BlendingJob::Layer>( layers, layers_iterator );
        blend_job.bind_pose = ozz_skeleton.bind_pose();
        blend_job.output = m_blendedLocals;

        // Blends.
        if( !blend_job.Run() )
        {
            return;
        }

        // Setup local-to-model conversion job.
        ozz::animation::LocalToModelJob ltm_job;
        ltm_job.skeleton = &ozz_skeleton;
        ltm_job.input = m_blendedLocals;
        ltm_job.output = m_models;

        // Run ltm job.
        if( !ltm_job.Run() ) {
            return;
        }

        const Detail::Mesh & ozz_mesh = m_resourceMesh->getMesh();

        Detail::Mesh::VectorJointRemaps::size_type joint_remap_count = ozz_mesh.joint_remaps.size();

        // Builds skinning matrices, based on the output of the animation stage.
        for( size_t i = 0; i != joint_remap_count; ++i )
        {
            uint16_t joint_index = ozz_mesh.joint_remaps[i];
            const ozz::math::Float4x4 & model = m_models[joint_index];

            const ozz::math::Float4x4 & bind_poses = ozz_mesh.inverse_bind_poses[i];

            m_skinningMatrices[i] = model * bind_poses;
        }

        // Renders skin.
        int32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

        // Positions and normals are interleaved to improve caching while executing
        // skinning job.

        uint32_t skinned_data_size = vertex_count * ozz_vertex_stride;

        // Reallocate vertex buffer.
        const uint32_t vbo_size = skinned_data_size;
        void * vbo_map = m_vertexMemory->newBuffer( vbo_size, "NodeOzzAnimation", __FILE__, __LINE__ );

        // Iterate mesh parts and fills vbo.
        // Runs a skinning job per mesh part. Triangle indices are shared
        // across parts.
        size_t processed_vertex_count = 0;
        for( const Detail::Part & part : ozz_mesh.parts )
        {
            // Skip this iteration if no vertex.
            size_t part_vertex_count = Detail::getPartVertexCount( part );

            if( part_vertex_count == 0 )
            {
                continue;
            }

            // Fills the job.
            ozz::geometry::SkinningJob skinning_job;
            skinning_job.vertex_count = static_cast<int32_t>(part_vertex_count);
            int32_t part_influences_count = getPartInfluencesCount( part );

            // Clamps joints influence count according to the option.
            skinning_job.influences_count = part_influences_count;

            // Setup skinning matrices, that came from the animation stage before being
            // multiplied by inverse model-space bind-pose.
            skinning_job.joint_matrices = m_skinningMatrices;

            // Setup joint's indices.
            skinning_job.joint_indices = ozz::make_range( part.joint_indices );
            skinning_job.joint_indices_stride = sizeof( uint16_t ) * part_influences_count;

            // Setup joint's weights.
            if( part_influences_count > 1 )
            {
                skinning_job.joint_weights = ozz::make_range( part.joint_weights );
                skinning_job.joint_weights_stride = sizeof( float ) * (part_influences_count - 1);
            }

            // Setup input positions, coming from the loaded mesh.
            skinning_job.in_positions = ozz::make_range( part.positions );
            skinning_job.in_positions_stride = sizeof( float ) * Detail::Part::kPositionsCpnts;

            // Setup output positions, coming from the rendering output mesh buffers.
            // We need to offset the buffer every loop.
            skinning_job.out_positions.begin = reinterpret_cast<float*>(
                ozz::PointerStride( vbo_map, ozz_positions_offset + processed_vertex_count *
                    ozz_vertex_stride ));
            skinning_job.out_positions.end = ozz::PointerStride(
                skinning_job.out_positions.begin, part_vertex_count * ozz_vertex_stride );
            skinning_job.out_positions_stride = ozz_vertex_stride;

            // Setup normals if input are provided.
            //float* out_normal_begin = reinterpret_cast<float*>(ozz::PointerStride(
            //    vbo_map, ozz_normals_offset + processed_vertex_count * ozz_vertex_stride ));
            //const float* out_normal_end = ozz::PointerStride(
            //    out_normal_begin, part_vertex_count * ozz_vertex_stride );

            // Setup input normals, coming from the loaded mesh.
            //skinning_job.in_normals = ozz::make_range( part.normals );
            //skinning_job.in_normals_stride =
                //sizeof( float ) * Detail::Part::kNormalsCpnts;

            // Setup output normals, coming from the rendering output mesh buffers.
            // We need to offset the buffer every loop.
            //skinning_job.out_normals.begin = out_normal_begin;
            //skinning_job.out_normals.end = out_normal_end;
            //skinning_job.out_normals_stride = ozz_vertex_stride;

            // Setup tangents if input are provided.
            //float* out_tangent_begin = reinterpret_cast<float*>(ozz::PointerStride(
            //    vbo_map, ozz_tangents_offset + processed_vertex_count * ozz_vertex_stride ));
            //const float* out_tangent_end = ozz::PointerStride(
            //    out_tangent_begin, part_vertex_count * ozz_vertex_stride );

            // Setup input tangents, coming from the loaded mesh.
            //skinning_job.in_tangents = ozz::make_range( part.tangents );
            //skinning_job.in_tangents_stride =
                //sizeof( float ) * Detail::Part::kTangentsCpnts;

            // Setup output tangents, coming from the rendering output mesh buffers.
            // We need to offset the buffer every loop.
            //skinning_job.out_tangents.begin = out_tangent_begin;
            //skinning_job.out_tangents.end = out_tangent_end;
            //skinning_job.out_tangents_stride = ozz_vertex_stride;


            // Execute the job, which should succeed unless a parameter is invalid.
            if( !skinning_job.Run() )
            {
                return;
            }

            Color color;
            this->calcTotalColor( color );

            // Handles colors which aren't affected by skinning.
            // Optimal path used when the right number of colors is provided.
            if( part.colors.empty() == false )
            {
                const uint8_t * part_colors_buffer = reinterpret_cast<const uint8_t *>(&part.colors[0]);
                for( uint8_t
                    *it = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_colors_offset,
                    *it_end = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_colors_offset;
                    it != it_end;
                    it += ozz_vertex_stride )
                {
                    uint8_t * vbo_colors_buffer = reinterpret_cast<uint8_t*>(it);

                    Helper::multiplyColorBuffer( color, vbo_colors_buffer, part_colors_buffer );

                    part_colors_buffer += ozz_colors_size;
                }
            }
            else
            {
                ColorValue_ARGB argb = color.getAsARGB();

                for( uint8_t
                    *it = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_colors_offset,
                    *it_end = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_colors_offset;
                    it != it_end;
                    it += ozz_vertex_stride )
                {
                    uint8_t * vbo_colors_buffer = reinterpret_cast<uint8_t*>(it);

                    memcpy( vbo_colors_buffer, &argb, ozz_colors_size );
                }
            }

            const uint8_t * part_uvs_buffer = reinterpret_cast<const uint8_t *>(&part.uvs[0]);
            for( uint8_t
                *it = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_uvs_offset,
                *it_end = reinterpret_cast<uint8_t*>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_uvs_offset;
                it != it_end;
                it += ozz_vertex_stride )
            {
                uint8_t * vbo_uvs_buffer = reinterpret_cast<uint8_t*>(it);

                memcpy( vbo_uvs_buffer, part_uvs_buffer, ozz_uvs_size );

                part_uvs_buffer += ozz_uvs_size;
            }

            // Some more vertices were processed.
            processed_vertex_count += part_vertex_count;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::render( const RenderContext * _context ) const
    {
        const Detail::Mesh & ozz_mesh = m_resourceMesh->getMesh();

        // Renders skin.
        int32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

        // Reallocate vertex buffer.
        uint32_t vbo_size = vertex_count;
        void * vbo_buffer = m_vertexMemory->getBuffer();

        struct OzzVertex
        {
            mt::vec3f position;
            uint32_t color;
            mt::vec2f uv;
        };
            
        m_vertexBuffer->draw( vbo_buffer, vbo_size, MENGINE_DOCUMENT_FUNCTION );

        const Detail::Mesh::VectorTriangleIndices & triangle_indices = ozz_mesh.triangle_indices;

        const uint16_t * triangle_indices_buffer_data = triangle_indices.data();
        size_t indices_count = triangle_indices.size();

        m_indexBuffer->draw( triangle_indices_buffer_data, indices_count, MENGINE_DOCUMENT_FUNCTION );

        const mt::mat4f & wm = this->getWorldMatrix();

        if( _context->transformation == nullptr )
        {
            m_renderWorldMatrix = wm;
        }
        else
        {
            const mt::mat4f & transformationWorldMatrix = _context->transformation->getTransformationWorldMatrix();

            m_renderWorldMatrix = wm * transformationWorldMatrix;
        }

        RenderContext new_context;
        new_context.viewport = _context->viewport;
        new_context.camera = _context->camera;
        new_context.transformation = this;
        new_context.scissor = _context->scissor;
        new_context.target = _context->target;

        this->addRenderMesh( &new_context, m_material, nullptr, m_vertexBuffer, m_indexBuffer, vertex_count, indices_count );
    }
}
