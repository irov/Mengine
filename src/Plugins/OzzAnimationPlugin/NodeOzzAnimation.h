#pragma once

#include "OzzAnimationInterface.h"

#include "Interface/MemoryInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "ResourceOzzSkeleton.h"
#include "ResourceOzzMesh.h"

#include "SamplerOzzAnimation.h"

#include "Kernel/Node.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/ResourceImage.h"

#include "ozz/base/memory/allocator.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/base/maths/simd_math.h"

namespace Mengine
{
    class NodeOzzAnimation
        : public Node
        , public BaseUpdation
        , public BaseRender
        , public RenderTransformationInterface
        , public UnknownNodeOzzAnimationInterface
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_UNKNOWABLE();

    public:
        NodeOzzAnimation();
        ~NodeOzzAnimation() override;

    public:
        void setResourceOzzImage( const ResourceImagePtr & _resource ) override;
        const ResourceImagePtr & getResourceOzzImage() const override;

        void setResourceOzzSkeleton( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzSkeleton() const override;

        void setResourceOzzMesh( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzMesh() const override;

    public:
        void addOzzAnimationSampler( const SamplerOzzAnimationInterfacePtr & _sampler ) override;
        void removeOzzAnimationSampler( const ConstString & _name ) override;

        const SamplerOzzAnimationInterfacePtr & findOzzAnimationSampler( const ConstString & _name ) const override;

        uint32_t getOzzAnimationSamplerCount() const override;
        const SamplerOzzAnimationInterfacePtr & getOzzAnimationSampler( uint32_t _index ) const override;

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderContext * _context ) const override;

    protected:
        void updateAnimation_();

    protected:
        ResourceImagePtr m_resourceImage;
        ResourceOzzSkeletonPtr m_resourceSkeleton;
        ResourceOzzMeshPtr m_resourceMesh;

        typedef Vector<SamplerOzzAnimationPtr> VectorSamplerOzzAnimations;
        VectorSamplerOzzAnimations m_samplerOzzAnimations;

        ozz::Range<ozz::math::SoaTransform> m_blendedLocals;

        ozz::Range<ozz::math::Float4x4> m_models;
        ozz::Range<ozz::math::Float4x4> m_skinningMatrices;

        ozz::Range<ozz::math::SimdFloat4> m_upperBodyJointWeights;

        int32_t m_upperBodyRoot;

        MemoryBufferInterfacePtr m_vertexMemory;
        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;

        RenderMaterialInterfacePtr m_material;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeOzzAnimation, Node> NodeOzzAnimationPtr;
}