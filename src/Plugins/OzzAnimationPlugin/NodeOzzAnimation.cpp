#include "NodeOzzAnimation.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "ResourceOzzAnimation.h"

#include "Kernel/RenderHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "OzzDetail.h"

#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/containers/vector.h"

#include "ozz/animation/runtime/skeleton_utils.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"

#include "ozz/geometry/runtime/skinning_job.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_positions_size = sizeof( float ) * 3;
    //static const uint32_t ozz_normals_size = sizeof( float ) * 3;
    //static const uint32_t ozz_tangents_size = sizeof( float ) * 4;
    //static const uint32_t ozz_colors_size = sizeof( uint8_t ) * 4;
    //static const uint32_t ozz_uvs_size = sizeof( float ) * 2;
    static constexpr uint32_t ozz_positions_size = sizeof( float ) * 3;
    static constexpr uint32_t ozz_colors_size = sizeof( uint8_t ) * 4;
    static constexpr uint32_t ozz_uvs_size = sizeof( float ) * 2;
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_vertex_stride = ozz_positions_size + ozz_normals_size + ozz_tangents_size + ozz_colors_size + ozz_uvs_size;
    static constexpr uint32_t ozz_vertex_stride = ozz_positions_size + ozz_colors_size + ozz_uvs_size;
    //////////////////////////////////////////////////////////////////////////
    //static const uint32_t ozz_positions_offset = 0;
    //static const uint32_t ozz_normals_offset = ozz_positions_offset + ozz_positions_size;
    //static const uint32_t ozz_tangents_offset = ozz_normals_offset + ozz_normals_size;
    //static const uint32_t ozz_colors_offset = ozz_tangents_offset + ozz_tangents_size;
    //static const uint32_t ozz_uvs_offset = ozz_colors_offset + ozz_colors_size;
    static constexpr uint32_t ozz_positions_offset = 0;
    static constexpr uint32_t ozz_colors_offset = ozz_positions_offset + ozz_positions_size;
    static constexpr uint32_t ozz_uvs_offset = ozz_colors_offset + ozz_colors_size;
    //////////////////////////////////////////////////////////////////////////
    NodeOzzAnimation::NodeOzzAnimation()
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

        this->recompile( [this, &_resource]()
        {
            m_resourceImage = _resource;

            if( m_resourceImage == nullptr )
            {
                return false;
            }

            return true;
        } );
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

        this->recompile( [this, &_resource]()
        {
            m_resourceSkeleton = _resource;

            if( m_resourceSkeleton == nullptr )
            {
                return false;
            }

            return true;
        } );
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

        this->recompile( [this, &_resource]()
        {
            m_resourceMesh = _resource;

            if( m_resourceMesh == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & NodeOzzAnimation::getResourceOzzMesh() const
    {
        return m_resourceMesh;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::addOzzAnimationSampler( const SamplerOzzAnimationInterfacePtr & _sampler )
    {
        m_samplers.emplace_back( _sampler );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::removeOzzAnimationSampler( const ConstString & _name )
    {
        VectorSamplerOzzAnimations::iterator it_found = Algorithm::find_if( m_samplers.begin(), m_samplers.end()
            , [_name]( const SamplerOzzAnimationPtr & _sampler )
        {
            return _sampler->getName() == _name;
        } );

        if( it_found == m_samplers.end() )
        {
            return;
        }

        m_samplers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerOzzAnimationInterfacePtr & NodeOzzAnimation::findOzzAnimationSampler( const ConstString & _name ) const
    {
        for( const SamplerOzzAnimationPtr & sampler : m_samplers )
        {
            if( sampler->getName() != _name )
            {
                continue;
            }

            return sampler;
        }

        return SamplerOzzAnimationInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NodeOzzAnimation::getOzzAnimationSamplerCount() const
    {
        VectorSamplerOzzAnimations::size_type count = m_samplers.size();

        return (uint32_t)count;
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerOzzAnimationInterfacePtr & NodeOzzAnimation::getOzzAnimationSampler( uint32_t _index ) const
    {
        const SamplerOzzAnimationPtr & sampler = m_samplers[_index];

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

        const ozz::animation::Skeleton & skeleton = m_resourceSkeleton->getOzzSkeleton();

        int32_t num_soa_joints = skeleton.num_soa_joints();
        int32_t num_joints = skeleton.num_joints();

        m_models.resize( num_joints );
        m_upperBodyJointWeights.resize( num_soa_joints );
        m_blendedLocals.resize( num_soa_joints );

        ozz::span<const Detail::Mesh> meshes = m_resourceMesh->getMeshes();

        size_t num_skinning_matrices = 0;
        for( const Detail::Mesh & mesh : meshes )
        {
            num_skinning_matrices =
                ozz::math::Max( num_skinning_matrices, mesh.joint_remaps.size() );
        }

        m_skinningMatrices.resize( num_skinning_matrices );

        for( const Detail::Mesh & mesh : meshes )
        {
            uint16_t highestJointIndex = Detail::getMeshHighestJointIndex( mesh );

            if( num_joints >= highestJointIndex )
            {
                continue;
            }

            LOGGER_ERROR( "node ozz animation '%s' compile error the provided mesh doesn't match skeleton (joint count mismatch)."
                , this->getName().c_str()
            );

            return false;
        }

        for( int32_t i = 0; i != num_soa_joints; ++i )
        {
            m_upperBodyJointWeights[i] = ozz::math::simd_float4::zero();
        }

        // Extracts the list of children of the shoulder.
        ozz::animation::IterateJointsDF( skeleton, [this]( int32_t _joint, int32_t )
        {
            float upper_body_joint_weight_setting = 1.f;

            // Updates upper body animation sampler joint weights.
            ozz::math::SimdFloat4 & weight_setting = m_upperBodyJointWeights[_joint / 4];

            weight_setting = ozz::math::SetI( weight_setting, ozz::math::simd_float4::Load1( upper_body_joint_weight_setting ), _joint % 4 );
        } );

        ozz::span<const Detail::Mesh> ozz_meshes = m_resourceMesh->getMeshes();

        m_renderMeshes.reserve( ozz_meshes.size() );

        for( const Detail::Mesh & ozz_mesh : ozz_meshes )
        {
            MemoryBufferInterfacePtr vertexMemory = MEMORY_SERVICE()
                ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( vertexMemory );

            RenderVertexBufferInterfacePtr vertexBuffer = RENDER_SYSTEM()
                ->createVertexBuffer( ozz_vertex_stride, BT_STREAM, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( vertexBuffer );

            uint32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

            vertexBuffer->resize( vertex_count );

            RenderIndexBufferInterfacePtr indexBuffer = RENDER_SYSTEM()
                ->createIndexBuffer( sizeof( RenderIndex ), BT_STREAM, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( indexBuffer );

            const Detail::Mesh::VectorTriangleIndices & triangle_indices = ozz_mesh.triangle_indices;

            Detail::Mesh::VectorTriangleIndices::size_type indices_count = triangle_indices.size();

            indexBuffer->resize( (uint32_t)indices_count );

            RenderMeshDesc desc;
            desc.vertexMemory = vertexMemory;
            desc.vertexBuffer = vertexBuffer;
            desc.indexBuffer = indexBuffer;

            m_renderMeshes.push_back( desc );
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture();

        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getMaterial( STRINGIZE_STRING_LOCAL( "Ozz_Texture_Blend" ), PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material );

        m_material = material;

        for( const SamplerOzzAnimationPtr & sampler : m_samplers )
        {
            if( sampler->compile() == false )
            {
                return false;
            }
        }

        if( this->updateAnimation_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::_release()
    {
        m_models.clear();
        m_skinningMatrices.clear();
        m_upperBodyJointWeights.clear();
        m_blendedLocals.clear();

        m_renderMeshes.clear();

        m_material = nullptr;

        for( const SamplerOzzAnimationPtr & sampler : m_samplers )
        {
            sampler->release();
        }

        m_samplers.clear();

        m_resourceImage->release();
        m_resourceSkeleton->release();
        m_resourceMesh->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::update( const UpdateContext * _context )
    {
        for( const SamplerOzzAnimationPtr & sampler : m_samplers )
        {
            sampler->update( _context );
        }

        this->updateAnimation_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeOzzAnimation::updateAnimation_()
    {
        ozz::animation::BlendingJob::Layer layers[32];
        uint32_t layers_iterator = 0;

        for( const SamplerOzzAnimationPtr & sampler : m_samplers )
        {
            MENGINE_ASSERTION_FATAL( layers_iterator != 32, "too many layers" );

            if( sampler->isCompile() == false )
            {
                continue;
            }

            const ozz::vector<ozz::math::SoaTransform> & locals = sampler->getLocals();
            float weight = sampler->getWeight();

            layers[layers_iterator].transform = ozz::make_span( locals );
            layers[layers_iterator].weight = weight;

            layers_iterator++;
        }

        float threshold = ozz::animation::BlendingJob().threshold;

        const ozz::animation::Skeleton & ozz_skeleton = m_resourceSkeleton->getOzzSkeleton();

        ozz::animation::BlendingJob blend_job;
        blend_job.threshold = threshold;
        blend_job.layers = {layers, layers_iterator};
        blend_job.rest_pose = ozz_skeleton.joint_rest_poses();
        blend_job.output = ozz::make_span( m_blendedLocals );

        // Blends.
        if( blend_job.Run() == false )
        {
            LOGGER_ERROR( "node ozz animation '%s' invalid blend job"
                , this->getName().c_str()
            );

            return false;
        }

        // Setup local-to-model conversion job.
        ozz::animation::LocalToModelJob ltm_job;
        ltm_job.skeleton = &ozz_skeleton;
        ltm_job.input = ozz::make_span( m_blendedLocals );
        ltm_job.output = ozz::make_span( m_models );

        // Run ltm job.
        if( ltm_job.Run() == false )
        {
            LOGGER_ERROR( "node ozz animation '%s' invalid local to model job"
                , this->getName().c_str()
            );

            return false;
        }

        ozz::span<const Detail::Mesh> ozz_meshes = m_resourceMesh->getMeshes();

        uint32_t ozz_mesh_index = 0;
        for( const Detail::Mesh & ozz_mesh : ozz_meshes )
        {
            const RenderMeshDesc & desc = m_renderMeshes[ozz_mesh_index++];

            Detail::Mesh::VectorJointRemaps::size_type mesh_joint_remap_count = ozz_mesh.joint_remaps.size();

            // Builds skinning matrices, based on the output of the animation stage.
            for( size_t i = 0; i != mesh_joint_remap_count; ++i )
            {
                uint16_t joint_index = ozz_mesh.joint_remaps[i];
                const ozz::math::Float4x4 & model = m_models[joint_index];

                const ozz::math::Float4x4 & bind_poses = ozz_mesh.inverse_bind_poses[i];

                m_skinningMatrices[i] = model * bind_poses;
            }

            // Renders skin.
            uint32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

            // Positions and normals are interleaved to improve caching while executing
            // skinning job.

            size_t skinned_data_size = vertex_count * ozz_vertex_stride;

            // Reallocate vertex buffer.
            size_t vbo_size = skinned_data_size;

            void * vbo_map = desc.vertexMemory->newBuffer( vbo_size );

            // Iterate mesh parts and fills vbo.
            // Runs a skinning job per mesh part. Triangle indices are shared
            // across parts.
            size_t processed_vertex_count = 0;
            for( const Detail::Part & part : ozz_mesh.parts )
            {
                // Skip this iteration if no vertex.
                uint32_t part_vertex_count = Detail::getPartVertexCount( part );

                if( part_vertex_count == 0 )
                {
                    continue;
                }

                // Fills the job.
                ozz::geometry::SkinningJob skinning_job;
                skinning_job.vertex_count = static_cast<int32_t>(part_vertex_count);
                uint32_t part_influences_count = Detail::getPartInfluencesCount( part );

                // Clamps joints influence count according to the option.
                skinning_job.influences_count = part_influences_count;

                // Setup skinning matrices, that came from the animation stage before being
                // multiplied by inverse model-space bind-pose.
                skinning_job.joint_matrices = ozz::make_span( m_skinningMatrices );

                // Setup joint's indices.
                skinning_job.joint_indices = ozz::make_span( part.joint_indices );
                skinning_job.joint_indices_stride = sizeof( uint16_t ) * part_influences_count;

                // Setup joint's weights.
                if( part_influences_count > 1 )
                {
                    skinning_job.joint_weights = ozz::make_span( part.joint_weights );
                    skinning_job.joint_weights_stride = sizeof( float ) * (part_influences_count - 1);
                }

                // Setup input positions, coming from the loaded mesh.
                skinning_job.in_positions = ozz::make_span( part.positions );
                skinning_job.in_positions_stride = sizeof( float ) * Detail::Part::kPositionsCpnts;

                // Setup output positions, coming from the rendering output mesh buffers.
                // We need to offset the buffer every loop.
                float * out_positions_begin = reinterpret_cast<float *>(
                    ozz::PointerStride( vbo_map, ozz_positions_offset + processed_vertex_count *
                        ozz_vertex_stride ));

                float * out_positions_end = ozz::PointerStride(
                    out_positions_begin, part_vertex_count * ozz_vertex_stride );

                skinning_job.out_positions = {out_positions_begin, out_positions_end};
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
                if( skinning_job.Run() == false )
                {
                    LOGGER_ERROR( "node ozz animation '%s' invalid skinning job"
                        , this->getName().c_str()
                    );

                    return false;
                }

                Color total_color;
                this->calcTotalColor( &total_color );

                // Handles colors which aren't affected by skinning.
                // Optimal path used when the right number of colors is provided.
                if( part.colors.empty() == false )
                {
                    const uint8_t * part_colors_buffer = reinterpret_cast<const uint8_t *>(part.colors.data());
                    for( uint8_t
                        * it = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_colors_offset,
                        *it_end = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_colors_offset;
                        it != it_end;
                        it += ozz_vertex_stride )
                    {
                        uint8_t * vbo_colors_buffer = reinterpret_cast<uint8_t *>(it);

                        Helper::multiplyColorBuffer( total_color, vbo_colors_buffer, part_colors_buffer );

                        part_colors_buffer += ozz_colors_size;
                    }
                }
                else
                {
                    ColorValue_ARGB argb = total_color.getAsARGB();

                    for( uint8_t
                        * it = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_colors_offset,
                        *it_end = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_colors_offset;
                        it != it_end;
                        it += ozz_vertex_stride )
                    {
                        uint8_t * vbo_colors_buffer = reinterpret_cast<uint8_t *>(it);

                        StdString::memcpy( vbo_colors_buffer, &argb, ozz_colors_size );
                    }
                }

                const uint8_t * part_uvs_buffer = reinterpret_cast<const uint8_t *>(part.uvs.data());
                for( uint8_t
                    * it = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + ozz_uvs_offset,
                    *it_end = reinterpret_cast<uint8_t *>(vbo_map) + processed_vertex_count * ozz_vertex_stride + part_vertex_count * ozz_vertex_stride + ozz_uvs_offset;
                    it != it_end;
                    it += ozz_vertex_stride )
                {
                    uint8_t * vbo_uvs_buffer = reinterpret_cast<uint8_t *>(it);

                    StdString::memcpy( vbo_uvs_buffer, part_uvs_buffer, ozz_uvs_size );

                    part_uvs_buffer += ozz_uvs_size;
                }

                // Some more vertices were processed.
                processed_vertex_count += part_vertex_count;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeOzzAnimation::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        ozz::span<const Detail::Mesh> ozz_meshes = m_resourceMesh->getMeshes();

        uint32_t ozz_mesh_index = 0;
        for( const Detail::Mesh & ozz_mesh : ozz_meshes )
        {
            const RenderMeshDesc & desc = m_renderMeshes[ozz_mesh_index++];

            // Renders skin.
            uint32_t vertex_count = Detail::getMeshVertexCount( ozz_mesh );

            // Reallocate vertex buffer.
            uint32_t vbo_size = vertex_count;
            void * vbo_buffer = desc.vertexMemory->getBuffer();

            desc.vertexBuffer->draw( vbo_buffer, 0, vbo_size );

            const Detail::Mesh::VectorTriangleIndices & triangle_indices = ozz_mesh.triangle_indices;

            const uint16_t * triangle_indices_buffer_data = triangle_indices.data();
            uint32_t indices_count = (uint32_t)triangle_indices.size();

            desc.indexBuffer->draw( triangle_indices_buffer_data, 0, indices_count );

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

            RenderContext new_context = *_context;

            new_context.transformation = this;

            _renderPipeline->addRenderMesh( &new_context, m_material, nullptr, desc.vertexBuffer, desc.indexBuffer, vertex_count, (uint32_t)indices_count, 0, 0, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
