#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "SpineInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "ResourceSpineSkeleton.h"
#include "SamplerSpineAnimation.h"

#include "spine/spine.h"

namespace Mengine
{
    class Spine
        : public Node
        , public UnknownSpineInterface
        , public BaseUpdation
        , public BaseRender
        , public BaseTransformation
    {
        DECLARE_FACTORABLE( Spine );
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Spine();
        ~Spine() override;

    public:
        void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) override;
        const ResourcePtr & getResourceSpineSkeleton() const override;

    public:
        bool addAnimationSampler( const SamplerSpineAnimationInterfacePtr & _sampler ) override;
        void removeAnimationSampler( const ConstString & _samplerName ) override;
        void clearAnimationSamplers() override;

        const SamplerSpineAnimationInterfacePtr & findAnimationSampler( const ConstString & _name ) const override;

        uint32_t getAnimationSamplerCount() const override;
        const SamplerSpineAnimationInterfacePtr & getAnimationSampler( uint32_t _index ) const override;

    public:
        bool getWorldBoundingBox( mt::box2f * _box ) const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool _afterActivate() override;

    protected:
        void fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColorValue_ARGB _argb, int _count, const mt::mat4f & _wm ) const;
        void fillIndices_( RenderIndex * _vertices2D, const RenderIndex * _triangles, uint32_t _count ) const;

    protected:
        RenderMaterialInterfacePtr makeMaterial_( spSlot * _slot, const ResourceImage * _resourceImage ) const;

    protected:
        ResourceSpineSkeletonPtr m_resourceSpineSkeleton;

        spSkeleton * m_skeleton;
        spSkeletonClipping * m_skeletonClipper;
        spAnimationStateData * m_animationStateData;

        typedef Vector<SamplerSpineAnimationPtr> VectorSamplerAnimations;
        VectorSamplerAnimations m_samplers;

        struct AttachmentMeshDesc
        {
            const ResourceImage * image = nullptr;
            RenderMaterialInterfacePtr material;

            VectorRenderVertex2D vertices;
            VectorRenderIndex indices;
        };

        typedef Vector<AttachmentMeshDesc> VectorAttachmentMesh;
        mutable VectorAttachmentMesh m_attachmentMeshes;

        struct AnimationEventDesc
        {
            int32_t trackIndex;
            spEventType type;

            ConstString state;
            ConstString name;

            const Char * eventName;
            int32_t eventIntValue;
            float eventFloatValue;
            const Char * eventStringValue;

            int32_t loopCount;
        };

        typedef Vector<AnimationEventDesc> VectorAnimationEvent;
        VectorAnimationEvent m_events;
        VectorAnimationEvent m_eventsAux;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Spine> SpinePtr;
    //////////////////////////////////////////////////////////////////////////
}